#!/bin/sh
# The next line restarts using tkscid: \
exec tclsh "$0" "$@"

###
### propagatelang.tcl
###

# (C) Pascal Georges 2007
#
# This is an utility script that helps adding keywords to each language file
# 1. It is supposed that language files are synchronized before running this script
# 2. The english file is compared to others and if tokens are in the english file and not in others
# it will added at the right place. The result is output to args.new
# Usage : "propagatelang.tcl francais" : will synchronize francais.tcl to english.tcl and send the output to francais.new
#      or "propagatelang.tcl" for all files
# "====== TODO To be translated ======" is inserted appropriately

source langList.tcl

proc checkfile {code langfile enc} {
  # Read this language file and the english file:

  set f [open english.tcl r]
  set data [read $f]
  close $f
  set englishData [split $data "\n"]
  set englishNames {}

  set f [open $langfile.tcl r]
  fconfigure $f -encoding $enc
  set data [read $f]
  close $f
  set langData [split $data "\n"]
  set langNames {}

  foreach line $langData {
    set fields [split $line]
    set command [lindex $fields 0]
    set lang [lindex $fields 1]
    set name [lindex $fields 2]
    if {$lang == $code  &&  ($command == "menuText" || $command == "translate" || $command == "helpMsg")} {
      lappend langNames $command:$name
    } else  {
      lappend langNames $line
    }
  }

  set fnew [open $langfile.tcl.new w]
  fconfigure $fnew -encoding $enc

  set lastLine -1
  foreach line $englishData {
    set fields [split $line]
    set command [lindex $fields 0]
    set lang [lindex $fields 1]
    set name [lindex $fields 2]

    if {$lang == "E"  &&  ($command == "menuText" || $command == "translate" || $command == "helpMsg")} {
      set lineCount [lsearch -exact $langNames $command:$name]
      if { $lineCount < 0} {
        puts $fnew  "# ====== TODO To be translated ======"
        puts $fnew [regsub " E " $line " $code "]
      } else {
        foreach l [lrange $langData [ expr $lastLine + 1 ] $lineCount] {
          puts $fnew $l
        }
        
        # in case of a \ at the end of the last line
        if {[string index $l end] == "\\"} {
          incr lineCount
          puts $fnew [lindex $langData $lineCount]
        }
        set lastLine $lineCount
      }
    }
  }

  foreach l [lrange $langData [ expr $lastLine + 1 ] end-1] {
    puts $fnew $l
  }
  close $fnew
}
################################################################################

if {[llength $argv] == 0} { set argv $languages }

foreach language $argv {
  if {[info exists codes($language)]} {
    checkfile $codes($language) $language $encodings($language)
  } else {
    puts "No such language file: $language"
  }
}

# end of file
