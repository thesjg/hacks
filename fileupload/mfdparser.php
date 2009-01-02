<?php

class MFDParser {

    public $Data;
    public $ElementData;

    public $Boundary;
    public $BoundaryMixed;

    public $ContentType;
    public $ContentDisposition;
    public $ContentTransferEncoding;

      /* Stats */
    public $ContentLength;
    public $BytesConsumed;

      /* Actions */
    private $ReadStartBoundary;
    private $CheckHeadersReq;

    public $POST;
    public $FILES;

    public function __construct($content_length, $boundary) {
        $this->ContentLength = (int)$content_length;
        $this->BytesConsumed = 0;

        $this->Data = '';
        $this->ElementData = false;

        $this->Boundary = $boundary;

        $this->ReadStartBoundary = true;
        $this->CheckHeadersReq = true;

        $this->POST = array();
        $this->FILES = array();
    }

    public function Consume($data) {

        $this->BytesConsumed += strlen($data);
        $this->Data .= $data;

        if ($this->ReadStartBoundary === true)
            if ($this->ReadToBoundary('--' . $this->Boundary . "\r\n") !== false)
                $this->ReadStartBoundary = false;

        while (true) {

            if ($this->CheckHeadersReq === true)
                $this->ParseHeaders();

              // Status may have changed in our call to ParseHeaders method
            if ($this->CheckHeadersReq === false) {
                $this->ElementData = $this->ReadToBoundary("\r\n--" . $this->Boundary);
                if ($this->ElementData !== false) {
                    $this->CheckHeadersReq = true;
                    print 'ElementData: ' . $this->ElementData . "\n";
                    $this->ElementData = false;
                } else {
                    return $this->BytesConsumed;
                }
            } else {
                return $this->BytesConsumed;
            }
        }
    }


    public function ReadToBoundary($boundary) {
        $pos = strpos($this->Data, $boundary);
        if ($pos !== false) {
            $data = substr($this->Data, 0, $pos);
            $this->Data = substr($this->Data, $pos + strlen($boundary));

            return $data;
        } else {
            return false;
        }
    }

    public function ParseHeaders() {
        $data = $this->ReadToBoundary("\r\n\r\n");
        if ($data === false) {
            $this->CheckHeadersReq = true;
            return;
        }

        $this->CheckHeadersReq = false;
        $lines = explode("\r\n", trim($data));
        foreach ($lines as $line) {

            list($header, $value) = explode(': ', $line);
            $header = strtolower($header);
            $value = trim($value);

            switch ($header) {
                case 'content-type':
                    $search_str = '; boundary=';
                    $search_str_off = strpos($value, $search_str);
                    if ($search_str_off === false)
                        $this->ContentType = $value;
                    else {
                        $this->ContentType = substr($value, 0, $search_str_off);
                        // XXX: Content-Type should always be multipart/mixed here, check
                        $this->BoundaryMixed = substr($value, $search_str_off+strlen($search_str));
                    }
                    break;
                case 'content-disposition':
                    $value_arr = explode(';', $value);
                    $this->ContentDisposition = array_shift($value_arr);
                    foreach ($value_arr as $value_sub) {
                        $value_sub = trim($value_sub);
                        list($value_sub_key, $value_sub_value) = explode('=', $value_sub);
                        switch ($value_sub_key) {
                            case 'name':
                                $this->Name = substr($value_sub_value, 1, -1);
                                break;
                            case 'filename':
                                $this->Filename = substr($value_sub_value, 1, -1);
                                break;
                        }
                    }
                    break;
                case 'content-transfer-encoding':
                    $this->ContentTransferEncoding = $value;
                    break;
            }
        }
    }
}

?>