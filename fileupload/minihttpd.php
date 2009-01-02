<?php

/*
Copyright (C) 2006-2007 Samuel J. Greear. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

require('clienthandler.php');
require('socketserver.php');
require('mfdparser.php');

$RequestBytesRead = 0;
$ContentLength = 0;
global $RequestBytesRead, $ContentLength;

class MiniHTTPD extends ClientHandler {

    public $Request;
    public $RequestSplit;
    public $RequestHeaders;
    public $RequestBytesRead;

    public $POSTBody;

    private $Callback;

    private $ContentLength;
    private $ContentType;
    private $MIMEBoundary;

    const REQUEST_TYPE_POST = 1;
    const REQUEST_TYPE_GET = 2;
    const REQUEST_TYPE_HEAD = 3;

    public function __construct($socket, $opts) {
        parent::__construct($socket);
        $this->Terminator = "\r\n\r\n";

        $this->Callback = $opts;

        $this->Request = NULL;
        $this->RequestSplit = NULL;
        $this->RequestHeaders = array();
        $this->RequestBytesRead = 0;

        $this->ContentLength = 0;
        $this->ContentType = NULL;
        $this->MIMEBoundary = NULL;
    }

    public function FoundTerminator() {
        $data = $this->ReduceInput();

        if ($this->Request === NULL) {
            $this->ParseRequest($data);
            if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_POST) {
                $this->ContentLength = (int)$this->RequestHeaders['content-length'];
                global $ContentLength;
                $ContentLength = $this->ContentLength;

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
                return;
            }
        }

          // All requests of type POST are an incoming file transfer
        if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_POST) {
            $this->RequestBytesRead = $this->MFDParser->Consume($data);
            global $RequestBytesRead;
            $RequestBytesRead = $this->RequestBytesRead;

            if ($this->RequestBytesRead >= $this->ContentLength) {
                $this->Write("HTTP/1.1 200 OK\r\n\r\n");
                call_user_func($this->Callback, $this->MFDParser->POST, $this->MFDParser->FILES, $this);
                $this->LingeringClose = true;
            }
        }

          // All requests of type GET are assumed to want to know the status of the transfer
        if ($this->RequestType == MiniHTTPD::REQUEST_TYPE_GET) {

            if ($this->RequestSplit[1] == '/favicon.ico') {
//                print "Request for favicon\n";
                $this->Write("HTTP/1.1 404 File not found\r\n");
            } else {
//                print "Request for status\n";
                global $ContentLength, $RequestBytesRead;

                if ($ContentLength === 0)
                    $percent_transferred = 0;
                else
                    $percent_transferred = round(((float)$RequestBytesRead/$ContentLength)*100);

                $this->Write("HTTP/1.1 200 OK\r\n\r\n");
                $this->Write($RequestBytesRead . ',' . $ContentLength .
                             ',' . $percent_transferred . "\r\n");
            }

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
        if (sizeof($lines) < 1)
            throw new SocketException('This does not appear to be a valid HTTP request');

        $first_line = array_shift($lines);
        $this->RequestSplit = explode(' ', $first_line);

        if ($this->RequestSplit[0] == 'POST')
            $this->RequestType = MiniHTTPD::REQUEST_TYPE_POST;
        elseif ($this->RequestSplit[0] == 'GET')
            $this->RequestType = MiniHTTPD::REQUEST_TYPE_GET;
        elseif ($this->RequestSplit[0] == 'HEAD')
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

?>
