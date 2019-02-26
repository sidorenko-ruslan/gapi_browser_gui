require 'socket'
require 'json'
require 'byebug'
require 'nokogiri'

class GapBrowser
	def initialize host, port
		@host = host
		@port = port
	end

	def goto url
		puts "goto: #{url}" 
		init_socket
		@sock.write({ 'command' => 'goto', 'command_data' => validate_url(url) }.to_json)
		reply_data = get_reply_data
		puts 'page loaded'
		reply_data
	end

	def page_html
		puts 'getting page html'
		init_socket
		@sock.write({ 'command' => 'page_html', 'command_data' => 'data' }.to_json)
		reply_data = get_reply_data

		unless reply_data == 'error'
			if File::exists?(reply_data)
				puts 'complete'
				Nokogiri::HTML(get_file_as_string(reply_data))
			end
		end
	end

	def get_element_data location
		puts 'getting element data'
		init_socket
		@sock.write({ 'command' => 'get_element_data', 'command_data' => location }.to_json)
		reply_data = get_reply_data
		unless reply_data == 'error'
			if File::exists?(reply_data)
				puts 'complete'
				Nokogiri::HTML(get_file_as_string(reply_data))
			end
		end
	end

	def perform_element_action location, action
		puts 'performing element action'
		init_socket
		@sock.write({ 'command' => 'perform_element_action', 'command_data' => "#{location};#{action}" }.to_json)
		reply_data = get_reply_data
		unless reply_data == 'error'
			if File::exists?(reply_data)
				puts 'complete'
				Nokogiri::HTML(get_file_as_string(reply_data))
			end
		end
	end

	private

	def init_socket
		@sock = TCPSocket.new(@host, @port)
	end

	def validate_url url
		# TODO
		url
	end

	def get_file_as_string(filename)
	  data = ''
	  f = File.open(filename, "r") 
	  f.each_line do |line|
	    data += line
	  end
	  return data
	end

	def get_reply_data
		reply_data = @sock.read
		@sock.close
		reply_data.chomp
	end
end