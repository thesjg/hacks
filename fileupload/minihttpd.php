#!/usr/local/bin/php
<?php

require_once('clienthandler.php');
require_once('socketserver.php');
require_once('mfdparser.php');


class MiniHTTPD extends ClientHandler {

    public $Request;
    public $RequestHeaders;
    public $RequestBytesRead;

    public $POSTBody;

    private $ContentLength;
    private $ContentType;
    private $MIMEBoundary;

    const REQUEST_TYPE_POST = 1;
    const REQUEST_TYPE_GET = 2;
    const REQUEST_TYPE_HEAD = 3;

    public function __construct($socket) {
        parent::__construct($socket);
        $this->Terminator = "\r\n\r\n";

        $this->Request = NULL;
        $this->RequestBytesRead = 0;
    }

    public function FoundTerminator() {
//print $this->InputData;
        $data = $this->ReduceInput();

//        print 'Found Terminator: ' . $this->Terminator . ' - Total Bytes Read: ' . $this->InputBytesRead . "\n";

        if ($this->Request === NULL) {
            $this->ParseRequest($data);
            if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_POST) {
                $this->ContentLength = (int)$this->RequestHeaders['content-length'];

                $search_str = '; boundary=';
                $search_str_off = strpos($this->RequestHeaders['content-type'], $search_str);
                if ($search_str_off === false)
                    throw new SocketException('No boundary in Content-Type header, not a multipart/form-data POST?');

                $this->ContentType = substr($this->RequestHeaders['content-type'], 0, $search_str_off);
                $this->MIMEBoundary = substr($this->RequestHeaders['content-type'], $search_str_off+strlen($search_str));

                if ($this->ContentType != 'multipart/form-data')
                    throw new SocketException('Not a multipart/form-data POST');

                $this->Terminator = NULL;
                $this->MFDParser = new MFDParser($this->ContentLength, $this->MIMEBoundary);
            }
            return;
        }

          // All requests of type POST are an incoming file transfer
        if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_POST) {
            $bytes_consumed = $this->MFDParser->Consume($data);
//print 'Bytes Consumed: ' . $bytes_consumed . "\n";
//print 'Content Length: ' . $this->ContentLength . "\n";

$this->Write('Bytes Consumed: ' . $bytes_consumed . "\n");

            if ($bytes_consumed >= $this->ContentLength) {
                $this->Write("\r\n\r\n");
                $this->LingeringClose = true;
            }
        }

          // All requests of type GET want to know the status of the transfer
        if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_GET) {
            $this->Write('STATUS OF THE UPLOAD IS BLA!');
            $this->Write("\r\n\r\n");
            $this->LingeringClose = true;
        }

          // We don't handle HEAD requests
        if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_HEAD) {
            $this->Write("\r\n\r\n");
            $this->LingeringClose = true;
        }

          // Standards want a 500 error here kthx
//        throw new SocketException('Unsupported request');
    }

    public function ParseRequest($data) {
        $this->Request = $data;

        $lines = explode("\r\n", $data);
        if (sizeof($lines) < 2)
            throw new SocketException('This does not appear to be a valid HTTP request');

        $first_line = array_shift($lines);
        $first_line_exploded = explode(' ', $first_line);
        if ($first_line_exploded[0] == 'POST')
            $this->RequestType = MiniHTTPD::REQUEST_TYPE_POST;
        elseif ($first_line_exploded == 'GET')
            $this->RequestType = MiniHTTPD::REQUEST_TYPE_GET;
        elseif ($first_line_exploded == 'HEAD')
            $this->RequestType = MiniHTTPD::REQUEST_TYPE_HEAD;
        else
            throw new SocketException('This does not appear to be a valid HTTP request');

        foreach ($lines as $line) {
            if (ctype_space($line) === true || $line == '')
                continue;

            list($header, $value) = explode(': ', $line);
            $this->RequestHeaders[strtolower($header)] = trim($value);
        }
    }

    public function ReduceInput() {
        $data = NULL;

        if ($this->Terminator === NULL) {
            $data = $this->InputData;
            $this->InputData = '';
        } else {
            $t_offset = strpos($this->InputData, $this->Terminator);
            if ($t_offset !== false) {
                $t_len = strlen($this->Terminator);
                $data = substr($this->InputData, 0, $t_offset + $t_len);
                $this->InputData = substr($this->InputData, $t_offset + $t_len);
            }
        }

        return $data;
    }
}

$ss = new SocketServer();
$ss->SetHandler('MiniHTTPD');
$ss->Setup('127.0.0.1', 54321);
$ss->Run();

?>