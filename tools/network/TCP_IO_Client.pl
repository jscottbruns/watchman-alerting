use IO::Socket::INET;

my $port = $ARGV[0] || 5000;
# flush after every write
$| = 1;
$/ = CRLF;

my ($socket,$client_socket);
my ($peeraddress,$peerport);

# creating object interface of IO::Socket::INET modules which internally does
# socket creation, binding and listening at the specified port address.
$socket = new IO::Socket::INET (
    LocalHost => '0.0.0.0',
    LocalPort => $port,
    Proto => 'tcp',
    Listen => 5,
    Reuse => 1
) or die "ERROR in Socket Creation \n";

print "SERVER Waiting for client connection on port $port\n";

while(1)
{
# waiting for new client connection.
$client_socket = $socket->accept();

# get the host and port number of newly connected client.
$peer_address = $client_socket->peerhost();
$peer_port = $client_socket->peerport();

print "Accepted New Client Connection From : $peeraddress, $peerport\n ";

# write operation on the newly accepted client.
#$data = "DATA from Server";
#print $client_socket "$data\n";
# we can also send the data through IO::Socket::INET module,
# $client_socket->send($data);

# read operation on the newly accepted client
chomp( $data = <$client_socket> );
# we can also read from socket through recv()  in IO::Socket::INET
# $client_socket->recv($data,1024);
print "Client Data Received: $data\n";
close $client_socket;
}

$socket->close();
