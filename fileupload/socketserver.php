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

require('socket.php');

class SocketServer extends Socket {

    public $Port;

    private $FDSet;

    private $Handler;
    private $HandlerOpts;
    private $Handlers;

    public function SetHandler($handler, $opts=NULL) {
        if (!class_exists($handler))
            throw new SocketException('Handler must be an instance of ClientHandler class');

        $this->Handler = $handler;
        $this->HandlerOpts = $opts;
    }

    public function Run($timeout=NULL, $timeout_callback=NULL) {

        $this->FDSet = array();
        $this->Handlers = array();

        while (true) {
            $read_set = array($this->Handle);
            $write_set = array();

            foreach ($this->FDSet as $active_fd) {
                if ($active_fd == $this->Handle)
                    continue;

                $client = $this->Handlers[$active_fd];
                if ($client->Readable === true)
                    $read_set[] = $client->Socket->Handle;

                if ($client->Writeable === true)
                    $write_set[] = $client->Socket->Handle;
            }

            try {
                $changed = $this->Select($read_set, $write_set, $timeout);
                if ($timeout !== NULL && $changed === 0)
                    $timeout_callback();
            } catch (SocketException $se) {
                  // XXX
                continue;
            }

            foreach ($read_set as $active_fd) {
                if ($active_fd == $this->Handle) {
                      // XXX: try
                    $new_socket = $this->Accept($this->Handle);
                    $new_client = new $this->Handler($new_socket, $this->HandlerOpts);

                    array_push($this->FDSet, $new_client->Socket->Handle);
                    $this->Handlers[$new_client->Socket->Handle] = $new_client;
                } else {
                    $client = $this->Handlers[$active_fd];
                    if ($client->HandleRead() === false || $client->Socket->Connected === false) {
                        $cindex = array_search($active_fd, $this->FDSet);
                        unset($this->FDSet[$cindex]);
                        unset($this->Handlers[$active_fd]);
                        $client->HandleClose();
                    }
                }
            }

            foreach ($write_set as $active_fd) {
                $client = $this->Handlers[$active_fd];
                if ($client->HandleWrite() === false) {
                    $client->HandleClose();
                }
            }
        }
    }

    public function Select(&$read_set, &$write_set, $timeout=0) {

        if (($changed = socket_select($read_set, $write_set, $except = NULL, $timeout)) === false)
            throw new SocketError('Error in select: ' . $this->StringError());

        return $changed;
    }

    public function Setup($address='0.0.0.0', $port=NULL, $nonblock=true,
                          $backlog=SOCKET_BACKLOG) {

        try {
            $this->SetNonBlock($nonblock);
            $this->SetOption(Socket::SO_REUSEADDR);
            $this->Bind($address, $port);
            list($address, $port) = $this->GetSocketName();
            $this->Port = $port;
        } catch (SocketException $se) {
            return false;
        }
    }
}

?>
