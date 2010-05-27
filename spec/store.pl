#use strict; 
use warnings; 

use Storable; 
use Data::Dumper; 

my %blah = (
            "a" => 123,
            "b" => -40
           );

#my $x =  "004\a\b12345678\004\b\b\b\003\004\000\000\000\004\003\002\000\000\000\n\004blah\001\000\000\000p\001>\002\000\000Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\001\000\000\000q\001\000\000\000c\n\00229\017\000\000\000longer_key_name\n\020A longish string\001\000\000\000a\n\00235\001\000\000\000b";

open(FOOBAR, ">foobar.txt");

print FOOBAR Storable::nfreeze(\%blah);


close FOOBAR
