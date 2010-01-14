require 'rcstorable'

describe RCStorable do


  it "should correctly decode format A" do
    store = "\005\006\003\000\000\000\005\005\000\000\000\aAaaaaaa\004\003\000\000\000\017\n\fasdjfsadfjfd\000\000\000\tdkjfjaksj\n\00560565\000\000\000\nksdjfksjdf\n\020aksjgjeijpqiwjff\000\000\000\020ksdjfkasjfkjefjj\n\022ewjfklqjkzxjfnvnds\000\000\000\ajfjwjwj\n\nlsakdvnnff\000\000\000\004qpwo\n\002jf\000\000\000\005qwojf\n\bzxemffew\000\000\000\005owjff\n\004zjxf\000\000\000\016jzxmfnfkneoqjf\n\003jas\000\000\000\aowfjjwf\n\003jfw\000\000\000\rjadsnzxfkjfej\n\005oqwfj\000\000\000\bqpwofjff\n\004owjf\000\000\000\njzxnfnqwwf\n\000\000\000\000\nqpwofjfjww\n\000\000\000\000\vpdofjfejqwj\n\002jf\000\000\000\vpwqofjwjqfj\000\000\000\apojsfww\005\000\000\000\005posjf\004\002\000\000\000\001\001\000\000\001?sdlkfjewigjepofjqwpfjsdalkfjqepogjgpojqwfpowejfqpowfjlksdjdslkgjsdflgjerhgi;oerhjgoiprejwgioperjgkl;djglsdk;fgjsdlkf;gjal;gfjerigjergkjarigojaegkl;sdfnga;lksfjwi;eogjieryjkw;ljfa;sdlgkhjaglkajwegf;lkajgaweklfhajklghawejklghawejkhawelurhaewtluiawehtawlkjehfasjkldfnasjklfhsadfjklashfluiewhwlquthqluitqwyeoiuthwefjklasdhfkjlasdfnbasjlkdfhqweuiltqytoeqwieutyqoeto9845yhsjdaghfajkewntewakjlhtawkerhweuirhaweruhewiurahwefljkshdafjhadkf\n\000\000\000\003oje\005\000\000\000\016lsdkfjlkfjqwtr"

    expected = {"Aaaaaaa"=>nil, "lsdkfjlkfjqwtr"=>nil, "posjf"=>nil, "oje"=>["sdlkfjewigjepofjqwpfjsdalkfjqepogjgpojqwfpowejfqpowfjlksdjdslkgjsdflgjerhgi;oerhjgoiprejwgioperjgkl;djglsdk;fgjsdlkf;gjal;gfjerigjergkjarigojaegkl;sdfnga;lksfjwi;eogjieryjkw;ljfa;sdlgkhjaglkajwegf;lkajgaweklfhajklghawejklghawejkhawelurhaewtluiawehtawlkjehfasjkldfnasjklfhsadfjklashfluiewhwlquthqluitqwyeoiuthwefjklasdhfkjlasdfnbasjlkdfhqweuiltqytoeqwieutyqoeto9845yhsjdaghfajkewntewakjlhtawkerhweuirhaweruhewiurahwefljkshdafjhadkf\n"], "pojsfww"=>{"jfjwjwj"=>"ewjfklqjkzxjfnvnds", "qpwo"=>"lsakdvnnff", "ksdjfksjdf"=>"60565", "pdofjfejqwj"=>"", "owfjjwf"=>"jas", "owjff"=>"zxemffew", "jzxnfnqwwf"=>"owjf", "qpwofjff"=>"oqwfj", "jadsnzxfkjfej"=>"jfw", "ksdjfkasjfkjefjj"=>"aksjgjeijpqiwjff", "pwqofjwjqfj"=>"jf", "dkjfjaksj"=>"asdjfsadfjfd", "qpwofjfjww"=>"", "jzxmfnfkneoqjf"=>"zjxf", "qwojf"=>"jf"}}

    RCStorable.thaw(store).should == expected
  end
    
  it "should correctly decode format B" do
    store = "004\a\b12345678\004\b\b\b\003\004\000\000\000\004\003\002\000\000\000\n\004blah\001\000\000\000p\001>\002\000\000Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\001\000\000\000q\001\000\000\000c\n\00229\017\000\000\000longer_key_name\n\020A longish string\001\000\000\000a\n\00235\001\000\000\000b"

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


end
