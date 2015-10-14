#!/usr/bin/env ruby

# 6 lines of code to validate command line arguments, open and read a file, 
# compute its CRC-32 checksum, and print it out.
# 
# Not really fair, is it? ;)
require 'zlib'

if ARGV.size != 1 || ! File.file?(ARGV[0])
  puts "USAGE: #{$0} FILE"
  exit -1
end

puts Zlib::crc32(File.read(ARGV[0]))
