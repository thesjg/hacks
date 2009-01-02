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

class ClientHandler {

    public $Socket;
    public $Terminator;

    public $LingeringClose;

    public $Readable;
    public $Writeable;

    public $InputData;
    public $InputBytesRead;

    public $OutputFifo;
    public $OutputOffset;
    public $OutputBytesWritten;

    public function __construct($socket) {
        $this->Socket = $socket;
        $this->Terminator = NULL;

        $this->LingeringClose = false;

        $this->Readable = true;
        $this->Writeable = false;

        $this->InputData = '';
        $this->InputBytesRead = 0;

        $this->OutputFifo = array();
        $this->OutputOffset = 0;
        $this->OutputBytesWritten = 0;
    }

    public function FoundTerminator() {
        throw new Exception('FoundTerminator method must be implemented in subclass');
    }

    public function Write($data) {
        if ($data == '' || $data == NULL)
            return;

        $this->Writeable = true;
        $this->OutputFifo[] = $data;
    }

    public function HandleRead() {

        try {
            list($cbytes, $cbuffer) = $this->Socket->Receive(4096); /* XXX */
            $this->InputBytesRead += $cbytes;

            while (strlen($cbuffer) > 0) {
                if ($this->Terminator === NULL) {
                    $this->InputData .= $cbuffer;
                    $cbuffer = '';
                    $this->FoundTerminator();
                } elseif (is_int($this->Terminator)) {
                    $input_data_len = strlen($this->InputData);
                    $cbuffer_len = strlen($cbuffer);

                    if ($input_data_len + $cbuffer_len < $this->Terminator) {
                        $this->InputData .= $cbuffer;
                        break;
                    }

                    $this->InputData .= substr($cbuffer, 0, $this->Terminator - $input_data_len);
                    $cbuffer = substr($cbuffer, $this->Terminator - $input_data_len);
                    $this->FoundTerminator();

                } else {
                    $term_off = strpos($cbuffer, $this->Terminator);
                    if ($term_off !== false) {
                        $term_off += strlen($this->Terminator);
                        $this->InputData .= substr($cbuffer, 0, $term_off);
                        $cbuffer = substr($cbuffer, $term_off);
                        $this->FoundTerminator();
                    } else {
                        $this->InputData .= $cbuffer;
                        $cbuffer = '';
                    }
                }

            }

        } catch (SocketException $se) {
//            print "Exception Caught in ClientHandler performing HandleRead\n";
            return false;
        }

        return true;
    }

    public function HandleWrite() {
        // XXX: Coalesce output bits

        if (!isset($this->OutputFifo[0]))
            throw new SocketException('XXX');

        try {

            while (sizeof($this->OutputFifo) > 0) {
                $cbuffer = substr($this->OutputFifo[0], $this->OutputOffset);
                $cbytes = $this->Socket->Send($cbuffer, strlen($cbuffer));
                $this->OutputBytesWritten += $cbytes;
                $this->OutputOffset += $cbytes;
                if ($this->OutputOffset == strlen($this->OutputFifo[0])) {
                    array_shift($this->OutputFifo);
                    $this->OutputOffset = 0;
                    if (sizeof($this->OutputFifo) == 0) {
                        $this->Writeable = false;
                        if ($this->LingeringClose === true)
                            $this->HandleClose();
                    }
                } else {
                    if ($this->LingeringClose === true)
                        $this->HandleClose();
                }
            }
        } catch (SocketException $se) {
//            print "Exception Caught in ClientHandler performing HandleWrite\n";
            return false;
        }

        return true;
    }

    public function HandleClose() {
//        print "Handling Close\n";
        $this->Readable = false;
        $this->Writeable = false;
        $this->Socket->__destruct();
    }
}

?>
