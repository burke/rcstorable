require 'rcstorable'

describe RCStorable do
  
  it "should understand Format 1286" do
    store = "\005\006\003\000\000\000\005\005\000\000\000\aAaaaaaa\004\003\000\000\000\017\n\fasdjfsadfjfd\000\000\000\tdkjfjaksj\n\00560565\000\000\000\nksdjfksjdf\n\020aksjgjeijpqiwjff\000\000\000\020ksdjfkasjfkjefjj\n\022ewjfklqjkzxjfnvnds\000\000\000\ajfjwjwj\n\nlsakdvnnff\000\000\000\004qpwo\n\002jf\000\000\000\005qwojf\n\bzxemffew\000\000\000\005owjff\n\004zjxf\000\000\000\016jzxmfnfkneoqjf\n\003jas\000\000\000\aowfjjwf\n\003jfw\000\000\000\rjadsnzxfkjfej\n\005oqwfj\000\000\000\bqpwofjff\n\004owjf\000\000\000\njzxnfnqwwf\n\000\000\000\000\nqpwofjfjww\n\000\000\000\000\vpdofjfejqwj\n\002jf\000\000\000\vpwqofjwjqfj\000\000\000\apojsfww\005\000\000\000\005posjf\004\002\000\000\000\001\001\000\000\001?sdlkfjewigjepofjqwpfjsdalkfjqepogjgpojqwfpowejfqpowfjlksdjdslkgjsdflgjerhgi;oerhjgoiprejwgioperjgkl;djglsdk;fgjsdlkf;gjal;gfjerigjergkjarigojaegkl;sdfnga;lksfjwi;eogjieryjkw;ljfa;sdlgkhjaglkajwegf;lkajgaweklfhajklghawejklghawejkhawelurhaewtluiawehtawlkjehfasjkldfnasjklfhsadfjklashfluiewhwlquthqluitqwyeoiuthwefjklasdhfkjlasdfnbasjlkdfhqweuiltqytoeqwieutyqoeto9845yhsjdaghfajkewntewakjlhtawkerhweuirhaweruhewiurahwefljkshdafjhadkf\n\000\000\000\003oje\005\000\000\000\016lsdkfjlkfjqwtr"

    expected = {"Aaaaaaa"=>nil, "lsdkfjlkfjqwtr"=>nil, "posjf"=>nil, "oje"=>["sdlkfjewigjepofjqwpfjsdalkfjqepogjgpojqwfpowejfqpowfjlksdjdslkgjsdflgjerhgi;oerhjgoiprejwgioperjgkl;djglsdk;fgjsdlkf;gjal;gfjerigjergkjarigojaegkl;sdfnga;lksfjwi;eogjieryjkw;ljfa;sdlgkhjaglkajwegf;lkajgaweklfhajklghawejklghawejkhawelurhaewtluiawehtawlkjehfasjkldfnasjklfhsadfjklashfluiewhwlquthqluitqwyeoiuthwefjklasdhfkjlasdfnbasjlkdfhqweuiltqytoeqwieutyqoeto9845yhsjdaghfajkewntewakjlhtawkerhweuirhaweruhewiurahwefljkshdafjhadkf\n"], "pojsfww"=>{"jfjwjwj"=>"ewjfklqjkzxjfnvnds", "qpwo"=>"lsakdvnnff", "ksdjfksjdf"=>"60565", "pdofjfejqwj"=>"", "owfjjwf"=>"jas", "owjff"=>"zxemffew", "jzxnfnqwwf"=>"owjf", "qpwofjff"=>"oqwfj", "jadsnzxfkjfej"=>"jfw", "ksdjfkasjfkjefjj"=>"aksjgjeijpqiwjff", "pwqofjwjqfj"=>"jf", "dkjfjaksj"=>"asdjfsadfjfd", "qpwofjfjww"=>"", "jzxmfnfkneoqjf"=>"zjxf", "qwojf"=>"jf"}}

    RCStorable.thaw(store).should == expected
  end
    
  it "Should understand Format 1287" do
    store = "\005\a\003\000\000\000\004\004\003\000\000\000\002\n\004blah\000\000\000\001p\001\000\000\002>Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\000\000\000\001q\000\000\000\001c\n\00229\000\000\000\017longer_key_name\n\020A longish string\000\000\000\001a\n\00235\000\000\000\001b"

    expected = {
      "a" => "A longish string",
      "b" => "35",
      "c" => {
        "p" => "blah", 
        "q" => "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."
      },
      "longer_key_name" => "29"
    }

    RCStorable.thaw(store).should == expected
  end

  it "should handle Format 1287 arrays" do 
    store = "\005\a\003\000\000\000\001\004\002\000\000\000\003\n\004asdf\n\004zxcv\n\004qwer\000\000\000\001b" 
    
    expected = {"b" => ["asdf", "zxcv", "qwer"]}

    RCStorable.thaw(store).should == expected
  end 

  it "should handle an empty hash" do
    store = "\005\a\003\000\000\000\001\004\003\000\000\000\000\000\000\000\001a" 
    
    expected = {"a" => {}}
    RCStorable.thaw(store).should == expected
  end 

  it "should understand numbers" do
    store = "\005\a\003\000\000\000\003\bX\000\000\000\001a\b\200\000\000\000\001b\b\373\000\000\000\001c" 
    expected = {"a" => -40, "b" => 0, "c" => 123}

    RCStorable.thaw(store).should == expected
  end 

  it "should understand nil" do
    store = "\005\a\003\000\000\000\001\005\000\000\000\001a" 
    expected = {"a" => nil}
    RCStorable.thaw(store).should == expected
  end 
  
end
