

require 'socket'
require 'json'

command = ARGV[0]
link = ARGV[1]
sock = TCPSocket.new('127.0.0.1', 3000)
sock.write({ "command" => command, 'command_data' => link }.to_json)
puts sock.read(15) # Since the response message has 5 bytes.
sock.close

