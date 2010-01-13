require 'rubygems'
require 'rake'

task :gem => :build

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "rcstorable"
    gem.summary = "Ruby C implementation of perl Storable's thaw"
    gem.description = "Super-fast Ruby C implementation of perl Storable's thaw"
    gem.email = "burke.libbey@canadadrugs.com"
    gem.homepage = "http://canadadrugs.com"
    gem.authors = ["Burke Libbey"]
    gem.files.include '{spec,lib,ext}/**/*'
    gem.extensions = ["ext/extconf.rb"]
  end
rescue LoadError
  puts "Jeweler (or a dependency) not available. Install it with: sudo gem install jeweler"
end

begin
  require 'rcov/rcovtask'
  Rcov::RcovTask.new do |test|
    test.libs << 'spec'
    test.pattern = 'spec/**/*_spec.rb'
    test.verbose = true
  end
rescue LoadError
  task :rcov do
    abort "RCov is not available. In order to run rcov, you must: sudo gem install spicycode-rcov"
  end
end

