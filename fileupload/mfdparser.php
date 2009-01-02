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

class MFDParser {

    public $Data;
    public $ElementData;

    public $Boundary;
    public $BoundaryMixed;

      /* Stats */
    public $ContentLength;
    public $BytesConsumed;

      /* Actions */
    private $ReadStartBoundary;
    private $CheckHeadersReq;

      /* Information about our current state */
    public $ContentType;
    public $ContentDisposition;
    public $ContentTransferEncoding;
    public $Name;
    public $Filename;

      /* Stored state */
    public $POST;
    public $FILES;

    public function __construct($content_length, $boundary) {
        $this->ContentLength = (int)$content_length;
        $this->BytesConsumed = 0;

        $this->Data = '';
        $this->ElementData = NULL;

        $this->Boundary = $boundary;

        $this->ReadStartBoundary = true;

        $this->POST = array();
        $this->FILES = array();

        $this->ResetState();
    }

    public function ResetState() {
        $this->CheckHeadersReq = true;
        $this->ContentType = NULL;
        $this->ContentDisposition = NULL;
        $this->ContentTransferEncoding = NULL;
        $this->Name = NULL;
        $this->Filename = NULL;
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
                    if ($this->Filename !== NULL) {
                        if ($this->ElementData === '') {
                            $this->StoreFileState(NULL);
                        } else {
                            $tmp_name = tempnam('./tmp', 'upload');

                            $handle = fopen($tmp_name, 'w');
                            fwrite($handle, $this->ElementData);
                            fclose($handle);

                            $this->StoreFileState($tmp_name);
                        }
                    } else {
                        $this->POST[$this->Name] = $this->ElementData;
                    }

                    $this->ResetState();
                    $this->ElementData = NULL;
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
            $this->ResetState();
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

    public function StoreFileState($tmp_name) {

        if ($tmp_name !== NULL)
            $state = array('name' => $this->Filename,
                           'type' => $this->ContentType,
                           'size' => filesize($tmp_name),
                           'tmp_name' => $tmp_name,
                           'error' => UPLOAD_ERR_OK);
        else
            $state = array('name' => NULL,
                           'type' => NULL,
                           'size' => 0,
                           'tmp_name' => NULL,
                           'error' => UPLOAD_ERR_NO_FILE);

        $this->FILES[$this->Name] = $state;

    }
}

?>
