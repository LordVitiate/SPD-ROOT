#!/usr/bin/perl

use Getopt::Std;
getopt('npus');

use POSIX qw(strftime);
$now_string = strftime "%Y/%m/%d", gmtime;

$singleton = $opt_s;
$classname = $opt_n;
$parent = $opt_p;

$classname or die "CreateNewClass: no name defined\n\t usage: CreateNewClass.pl -n name [-u <username>]";

$uid = $<;
$user = (getpwuid($uid))[6];
$user=~s/,.*$//;

if ($opt_u) {
  $user = $opt_u;
}

$aa = $classname;
$aa=~y/[a-z]/[A-Z]/;

$ifd = "__".$aa."_H__";
$inc = "";
$classparent = TObject;

print "creating $classname.h\n";
open(HH,">$classname.h");
print HH <<pippo;
// \$Id\$
// Author: $user   $now_string

\#ifndef $ifd
\#define $ifd

\#include <$classparent.h>
$inc
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// $classname                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class $classname: public $classparent {

public:

    $classname();
    virtual ~$classname();

private:

    ClassDef($classname,1)
};

\#endif  /* $ifd */

pippo

close(HH);

print "creating $classname.cxx\n";
open(CC,">$classname.cxx");
print CC <<pippo;
// \$Id\$
// Author: $user   $now_string


//_____________________________________________________________________________
//
// $classname
//_____________________________________________________________________________

\#include "$classname.h"

ClassImp($classname)

//_____________________________________________________________________________
$classname\:\:$classname() 
{
 
}

//_____________________________________________________________________________
$classname\:\:~$classname() 
{
 
}

pippo


