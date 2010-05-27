require 'rocco'
File.open('rcstorable.html','w'){|f|f.puts(Rocco.new('rcstorable.c',[],:language => 'c',:comment_pattern=>/^\s*\/?\*\/?/).to_html)}
