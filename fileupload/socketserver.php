<?php

require_once('socket.php');

class SocketServer extends Socket {

    public $Port;

    private $FDSet;

    private $Handler;
    private $Handlers;

    public function SetHandler($handler) {
        if (!class_exists($handler))
            throw new SocketException('Handler must be an instance of ClientHandler class');

        $this->Handler = $handler;
    }

    public function Run($timeout=NULL) {

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
            } catch (SocketException $se) {
                  // XXX
                continue;
            }

            foreach ($read_set as $active_fd) {
                if ($active_fd == $this->Handle) {
                      // XXX: try
print "XXX: Doing Accept\n";
                    $new_socket = $this->Accept($this->Handle);
                    $new_client = new $this->Handler($new_socket);

                    array_push($this->FDSet, $new_client->Socket->Handle);
                    $this->Handlers[$new_client->Socket->Handle] = $new_client;
                } else {
                    $client = $this->Handlers[$active_fd];
                    if ($client->HandleRead() === false) {
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
            $this->Bind($address, $port);
            $this->SetNonBlock($nonblock);
            $this->SetOption(Socket::SO_REUSEADDR);
            $this->Listen();
            list($address, $port) = $this->GetSocketName();
            $this->Port = $port;
        } catch (SocketException $se) {
            return false;
        }
    }
}

?>