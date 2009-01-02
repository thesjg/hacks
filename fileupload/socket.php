<?php

define('SOCKET_BACKLOG', 128);

class SocketException extends Exception {}

class Socket {

      /* Protocol Families */
    const PF_LOCAL = AF_UNIX;
    const PF_INET = AF_INET;
    const PF_INET6 = AF_INET6;

      /* Address Families */
    const AF_LOCAL = AF_UNIX;
    const AF_INET = AF_INET;
    const AF_INET6 = AF_INET6;

      /* Socket Types */
    const SOCK_STREAM = SOCK_STREAM;
    const SOCK_DGRAM = SOCK_DGRAM;

      /* Socket Options */
    const SO_REUSEADDR = SO_REUSEADDR;

    public $Handle;

    private $Connected = false;
    private $Bound = false;

    public function __construct($domain=Socket::PF_INET, $type=Socket::SOCK_STREAM) {
        if ($domain == NULL && $type == NULL)
            return;

        $this->Handle = socket_create($domain, $type, 0);
        if ($this->Handle < 0)
            throw new SocketException('Could not create socket: ' .
                                      $this->StringError());
    }

    public function __destruct() {
          /* Shutdown socket reading and writing */
        socket_shutdown($this->Handle, 2);
        socket_close($this->Handle);
    }

    public function Accept() {
        if (($handle = socket_accept($this->Handle)) === false)
            throw new SocketException('Could not accept connection on socket: ' .
                                      $this->StringError());

        $socket = new Socket(NULL, NULL);
        $socket->Handle = $handle;
        $socket->Connected = true;
        return $socket;
    }

    public function Bind($address, $port=NULL) {
        if ($this->Connected == true)
            throw new SocketException('Bind must be called on a socket prior to Connect');

        if (socket_bind($this->Handle, $address, $port) === false)
            throw new SocketException('Could not bind socket to address: ' .
                                      $this->StringError());
        $this->Bound = true;
    }

    public function Connect($address, $port=NULL) {
        if (socket_connect($this->Handle, $address, $port) === false)
            throw new SocketException('Socket could not connect to ' . $address .
                                      ': ' . $this->StringError());
        $this->Connected = true;
    }

    public function Listen($backlog=SOCKET_BACKLOG) {
        if ($this->Bound == false)
            throw new SocketException('Bind must be called on a socket prior to Listen');

        if (socket_listen($this->Handle, $backlog) === false)
            throw new SocketException('Could not listen on socket: ' .
                                      $this->StringError());
    }


    public function GetSocketName() {
        $address = $port = NULL;

        if (socket_getsockname($this->Handle, $address, $port) === false)
            throw new SocketException('Could not retreive socket name: ' .
                                      $this->StringError());

        return array($address, $port);
    }

    public function GetOption($optname) {
        if (socket_get_option($this->Handle, SOL_SOCKET, $optname) === false)
            throw new SocketException('Could not retreive socket option: ' .
                                      $this->StringError());
    }

    public function SetOption($optname, $optval=1) {
        if (socket_set_option($this->Handle, SOL_SOCKET, $optname, $optval) === false)
            throw new SocketException('Could not set socket option: ' .
                                      $this->StringError());
    }

    public function SetNonBlock($tf) {
        if (!is_bool($tf))
            throw new SocketException('SetNonBlock parameter must be boolean');

        if ($tf === true)
            $result = socket_set_nonblock($this->Handle);
        elseif ($tf === false)
            $result = socket_set_block($this->Handle);

        if ($result === false)
            throw new SocketExceoption('Blocking mode could not be set on socket: ' .
                                       $this->StringError());
    }

    public function Receive($length=16384) {
        $buffer = NULL;
        $bytes = 0;

        if (($bytes = socket_recv($this->Handle, $buffer, $length, 0)) <= 0)
            if ($bytes == 0)
                throw new SocketException('Remote closed the connection: ' .
                                          $this->StringError());
            else
                throw new SocketException('Unable to receive data from socket: ' .
                                          $this->StringError());

        return array($bytes, $buffer);
    }

    public function Send($data, $length=NULL) {
        if ($length === NULL)
            $length = strlen($data);

        if (($bytes = socket_send($this->Handle, $data, $length, 0)) < 0)
            throw new SocketException('Unable to send data on socket: ' .
                                      $this->StringError());

        return $bytes;
    }

    public function Read($length=16384) {
        if (($data = socket_read($this->Handle, $length)) === false)
            throw new SocketException('Could not read from socket: ' .
                                      $this->StringError());
        return $data;
    }

    public function Write($data, $length=NULL) {
        if (($written = socket_write($this->Handle, $length)) === false)
            throw new SocketException('Could not write to socket: ' .
                                      $this->StringError());
        return $written;
    }


    public function StringError() {
        return socket_strerror(socket_last_error($this->Handle));
    }
}

?>
