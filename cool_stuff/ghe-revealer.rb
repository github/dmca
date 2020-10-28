#!/usr/bin/sudo ruby

# 
# revealer.rb -- Deobfuscate GHE .rb files.
# 
# This is simple:
# Every obfuscated file in the GHE VM contains the following code:
# 
# > require "ruby_concealer.so"
# > __ruby_concealer__ "..."
# 
# GHE uses a C extension (ruby_concealer.so) which defines a global
# method named `__ruby_concealer__`. The string passed to this method
# is a string XORed with a "key" and then deflated using `Zlib::Deflate.deflate`.
# We just need to do it in reverse in order to get readable source code.
# 
# This code is quite fragile, but it was made just for the fun of learning
# a bit about Ruby and the GitHub Enterprise codebase. Besides, it does
# the job.

require 'zlib'

if ARGV.length != 1 or !File.directory?(ARGV[0]) then
    puts "Usage: #{$0} <ghe-directory>"
else
    fnum = 0
    processed = 0
    key = "This obfuscation is intended to discourage GitHub Enterprise customers from making modifications to the VM. We know this 'encryption' is easily broken. ".bytes.to_a
    Dir.glob("#{ARGV[0]}/**/*.rb") { |fname|
        fnum += 1
        s = File.open(fname, "r") { |f|
            begin
                next unless File.readlines(f).grep(/__ruby_concealer__/).any?
                eval(f.readline.sub(/__ruby_concealer__/, ''))
            rescue 
                next
            end
        }
        next if !s
        puts "Processing #{fname}..."
        uc = Zlib::Inflate.inflate(s)
        File.open(fname, "w") { |of|
            of.write(uc.bytes.each_with_index.map{ |c,i| (c ^ key[i % key.length]).chr }.join)
        }
        processed += 1
    }
    puts "Done. #{fnum} files found, #{processed} of which were processed."
end
