require 'rcstorable'

describe RCStorable do

  it "should correctly decode output" do
    input = "\005\006#{File.read(File.join(File.dirname(__FILE__),'test.store'))}"

    puts RCStorable.thaw(input).inspect
    
  end


end
