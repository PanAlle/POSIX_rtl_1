# POSIX_rtl_1
Message queues
Write two programs, a ”client” and a ”server”.  The client cansend a string message to the server and the server then respondswith a message (both:  max size 256 byte).Make sure that your design supports multiple clients connecting tothe same server, and each client only receiving the messages it issupposed to.Close and deregister queues when the program is halted by SIGINT(ctrl-c)
