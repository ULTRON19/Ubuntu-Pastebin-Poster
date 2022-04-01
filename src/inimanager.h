#ifndef __INIMANAGER__
#define __INIMANAGER__

#define ININAME_SETUP		"setup.ini"

#include <algorithm>

#include "comboboxitem.h"
#include "resmanager.h"
#include "wincvt.h"

class INIMANAGER
{
public:
	static INIMANAGER* GetInstance ();
	
	bool Initialize (std::string sFolderPath);
	bool Save ();
	
	std::string GetPosterName () const;
	const std::vector <SYNTAXITEM>& GetSyntaxList () const;
	const std::vector <EXPIRATIONITEM>& GetExpirationList () const;
	
	void UpdatePosterName (std::string _sPosterName);
	void UpdateSyntaxList (std::string sSuffix);
	bool UpdateSyntaxList (unsigned int idx);
	bool UpdateExpirationList (unsigned int idx);

private:
	INIMANAGER () = default;
	
	bool ExtractIniFromResource ();
	bool LoadPosterNameFromIni ();
	bool SavePosterNameToIni ();
	
	std::string sPosterName;
	std::vector <SYNTAXITEM> vSyntaxList;
	std::vector <EXPIRATIONITEM> vExpirationList;

	std::string sFileName;
};

static const std::initializer_list <std::pair <std::string, std::string>> psSyntaxPairList =
{
	{"Plain Text", "text"}, 
	{"ABAP", "abap"}, 
	{"ABNF", "abnf"}, 
	{"Ada", "ada"}, 
	{"ADL", "adl"}, 
	{"Agda", "agda"}, 
	{"Aheui", "aheui"}, 
	{"autohotkey", "ahk"}, 
	{"Alloy", "alloy"}, 
	{"Ampl", "ampl"}, 
	{"ANTLR", "antlr"}, 
	{"ANTLR With ActionScript Target", "antlr-as"}, 
	{"ANTLR With CPP Target", "antlr-cpp"}, 
	{"ANTLR With C# Target", "antlr-csharp"}, 
	{"ANTLR With Java Target", "antlr-java"}, 
	{"ANTLR With ObjectiveC Target", "antlr-objc"}, 
	{"ANTLR With Perl Target", "antlr-perl"}, 
	{"ANTLR With Python Target", "antlr-python"}, 
	{"ANTLR With Ruby Target", "antlr-ruby"}, 
	{"ApacheConf", "apacheconf"}, 
	{"APL", "apl"}, 
	{"AppleScript", "applescript"}, 
	{"Arduino", "arduino"}, 
	{"ActionScript", "as"}, 
	{"ActionScript 3", "as3"}, 
	{"AspectJ", "aspectj"}, 
	{"aspx-cs", "aspx-cs"}, 
	{"aspx-vb", "aspx-vb"}, 
	{"Asymptote", "asy"}, 
	{"AmbientTalk", "at"}, 
	{"AutoIt", "autoit"}, 
	{"Awk", "awk"}, 
	{"Base Makefile", "basemake"}, 
	{"Bash", "bash"}, 
	{"Batchfile", "bat"}, 
	{"BBCode", "bbcode"}, 
	{"BC", "bc"}, 
	{"Befunge", "befunge"}, 
	{"BibTeX", "bib"}, 
	{"BlitzBasic", "blitzbasic"}, 
	{"BlitzMax", "blitzmax"}, 
	{"BNF", "bnf"}, 
	{"Boo", "boo"}, 
	{"Boogie", "boogie"}, 
	{"Bro", "bro"}, 
	{"BST", "bst"}, 
	{"BUGS", "bugs"}, 
	{"C", "c"}, 
	{"c-objdump", "c-objdump"}, 
	{"ca65 assembler", "ca65"}, 
	{"cADL", "cadl"}, 
	{"CAmkES", "camkes"}, 
	{"CapDL", "capdl"}, 
	{"Cap'n Proto", "capnp"}, 
	{"CBM BASIC V2", "cbmbas"}, 
	{"Ceylon", "ceylon"}, 
	{"Coldfusion CFC", "cfc"}, 
	{"CFEngine3", "cfengine3"}, 
	{"Coldfusion HTML", "cfm"}, 
	{"cfstatement", "cfs"}, 
	{"ChaiScript", "chai"}, 
	{"Chapel", "chapel"}, 
	{"Cheetah", "cheetah"}, 
	{"Cirru", "cirru"}, 
	{"Clay", "clay"}, 
	{"Clean", "clean"}, 
	{"Clojure", "clojure"}, 
	{"ClojureScript", "clojurescript"}, 
	{"CMake", "cmake"}, 
	{"COBOL", "cobol"}, 
	{"COBOLFree", "cobolfree"}, 
	{"CoffeeScript", "coffee-script"}, 
	{"Common Lisp", "common-lisp"}, 
	{"Component Pascal", "componentpascal"}, 
	{"Bash Session", "console"}, 
	{"Debian Control file", "control"}, 
	{"Coq", "coq"}, 
	{"C++", "cpp"}, 
	{"cpp-objdump", "cpp-objdump"}, 
	{"CPSA", "cpsa"}, 
	{"Crystal", "cr"}, 
	{"Crmsh", "crmsh"}, 
	{"Croc", "croc"}, 
	{"Cryptol", "cryptol"}, 
	{"C#", "csharp"}, 
	{"Csound Orchestra", "csound"}, 
	{"Csound Document", "csound-document"}, 
	{"Csound Score", "csound-score"}, 
	{"CSS", "css"}, 
	{"CSS+Django/Jinja", "css+django"}, 
	{"CSS+Ruby", "css+erb"}, 
	{"CSS+Genshi Text", "css+genshitext"}, 
	{"CSS+Lasso", "css+lasso"}, 
	{"CSS+Mako", "css+mako"}, 
	{"CSS+mozpreproc", "css+mozpreproc"}, 
	{"CSS+Myghty", "css+myghty"}, 
	{"CSS+PHP", "css+php"}, 
	{"CSS+Smarty", "css+smarty"}, 
	{"Gherkin", "cucumber"}, 
	{"CUDA", "cuda"}, 
	{"Cypher", "cypher"}, 
	{"Cython", "cython"}, 
	{"D", "d"}, 
	{"d-objdump", "d-objdump"}, 
	{"Dart", "dart"}, 
	{"Delphi", "delphi"}, 
	{"dg", "dg"}, 
	{"Diff", "diff"}, 
	{"Django/Jinja", "django"}, 
	{"Docker", "docker"}, 
	{"MSDOS Session", "doscon"}, 
	{"Darcs Patch", "dpatch"}, 
	{"DTD", "dtd"}, 
	{"Duel", "duel"}, 
	{"Dylan", "dylan"}, 
	{"Dylan session", "dylan-console"}, 
	{"DylanLID", "dylan-lid"}, 
	{"Earl Grey", "earl-grey"}, 
	{"Easytrieve", "easytrieve"}, 
	{"EBNF", "ebnf"}, 
	{"eC", "ec"}, 
	{"ECL", "ecl"}, 
	{"Eiffel", "eiffel"}, 
	{"Elixir", "elixir"}, 
	{"Elm", "elm"}, 
	{"EmacsLisp", "emacs"}, 
	{"ERB", "erb"}, 
	{"Erlang erl session", "erl"}, 
	{"Erlang", "erlang"}, 
	{"Evoque", "evoque"}, 
	{"xtlang", "extempore"}, 
	{"Ezhil", "ezhil"}, 
	{"Factor", "factor"}, 
	{"Fantom", "fan"}, 
	{"Fancy", "fancy"}, 
	{"Felix", "felix"}, 
	{"Fish", "fish"}, 
	{"Flatline", "flatline"}, 
	{"Forth", "forth"}, 
	{"Fortran", "fortran"}, 
	{"FortranFixed", "fortranfixed"}, 
	{"FoxPro", "foxpro"}, 
	{"FSharp", "fsharp"}, 
	{"GAP", "gap"}, 
	{"GAS", "gas"}, 
	{"Genshi", "genshi"}, 
	{"Genshi Text", "genshitext"}, 
	{"GLSL", "glsl"}, 
	{"Gnuplot", "gnuplot"}, 
	{"Go", "go"}, 
	{"Golo", "golo"}, 
	{"GoodData-CL", "gooddata-cl"}, 
	{"Gosu", "gosu"}, 
	{"Groff", "groff"}, 
	{"Groovy", "groovy"}, 
	{"Gosu Template", "gst"}, 
	{"Haml", "haml"}, 
	{"Handlebars", "handlebars"}, 
	{"Haskell", "haskell"}, 
	{"Hxml", "haxeml"}, 
	{"Hexdump", "hexdump"}, 
	{"HSAIL", "hsail"}, 
	{"HTML", "html"}, 
	{"HTML+Cheetah", "html+cheetah"}, 
	{"HTML+Django/Jinja", "html+django"}, 
	{"HTML+Evoque", "html+evoque"}, 
	{"HTML+Genshi", "html+genshi"}, 
	{"HTML+Handlebars", "html+handlebars"}, 
	{"HTML+Lasso", "html+lasso"}, 
	{"HTML+Mako", "html+mako"}, 
	{"HTML+Myghty", "html+myghty"}, 
	{"HTML + Angular2", "html+ng2"}, 
	{"HTML+PHP", "html+php"}, 
	{"HTML+Smarty", "html+smarty"}, 
	{"HTML+Twig", "html+twig"}, 
	{"HTML+Velocity", "html+velocity"}, 
	{"HTTP", "http"}, 
	{"Haxe", "hx"}, 
	{"Hybris", "hybris"}, 
	{"Hy", "hylang"}, 
	{"Inform 6 template", "i6t"}, 
	{"IDL", "idl"}, 
	{"Idris", "idris"}, 
	{"Elixir iex session", "iex"}, 
	{"Igor", "igor"}, 
	{"Inform 6", "inform6"}, 
	{"Inform 7", "inform7"}, 
	{"INI", "ini"}, 
	{"Io", "io"}, 
	{"Ioke", "ioke"}, 
	{"IRC logs", "irc"}, 
	{"Isabelle", "isabelle"}, 
	{"J", "j"}, 
	{"JAGS", "jags"}, 
	{"Jasmin", "jasmin"}, 
	{"Java", "java"}, 
	{"Javascript+mozpreproc", "javascript+mozpreproc"}, 
	{"JCL", "jcl"}, 
	{"Julia console", "jlcon"}, 
	{"JavaScript", "js"}, 
	{"JavaScript+Cheetah", "js+cheetah"}, 
	{"JavaScript+Django/Jinja", "js+django"}, 
	{"JavaScript+Ruby", "js+erb"}, 
	{"JavaScript+Genshi Text", "js+genshitext"}, 
	{"JavaScript+Lasso", "js+lasso"}, 
	{"JavaScript+Mako", "js+mako"}, 
	{"JavaScript+Myghty", "js+myghty"}, 
	{"JavaScript+PHP", "js+php"}, 
	{"JavaScript+Smarty", "js+smarty"}, 
	{"JSGF", "jsgf"}, 
	{"JSON", "json"}, 
	{"JSONBareObject", "json-object"}, 
	{"JSON-LD", "jsonld"}, 
	{"Java Server Page", "jsp"}, 
	{"Julia", "julia"}, 
	{"Juttle", "juttle"}, 
	{"Kal", "kal"}, 
	{"Kconfig", "kconfig"}, 
	{"Koka", "koka"}, 
	{"Kotlin", "kotlin"}, 
	{"Literate Agda", "lagda"}, 
	{"Lasso", "lasso"}, 
	{"Literate Cryptol", "lcry"}, 
	{"Lean", "lean"}, 
	{"LessCss", "less"}, 
	{"Literate Haskell", "lhs"}, 
	{"Literate Idris", "lidr"}, 
	{"Lighttpd configuration file", "lighty"}, 
	{"Limbo", "limbo"}, 
	{"liquid", "liquid"}, 
	{"LiveScript", "live-script"}, 
	{"LLVM", "llvm"}, 
	{"Logos", "logos"}, 
	{"Logtalk", "logtalk"}, 
	{"LSL", "lsl"}, 
	{"Lua", "lua"}, 
	{"Makefile", "make"}, 
	{"Mako", "mako"}, 
	{"MAQL", "maql"}, 
	{"Mask", "mask"}, 
	{"Mason", "mason"}, 
	{"Mathematica", "mathematica"}, 
	{"Matlab", "matlab"}, 
	{"Matlab session", "matlabsession"}, 
	{"markdown", "md"}, 
	{"MiniD", "minid"}, 
	{"Modelica", "modelica"}, 
	{"Modula-2", "modula2"}, 
	{"Monkey", "monkey"}, 
	{"Monte", "monte"}, 
	{"MOOCode", "moocode"}, 
	{"MoonScript", "moon"}, 
	{"mozhashpreproc", "mozhashpreproc"}, 
	{"mozpercentpreproc", "mozpercentpreproc"}, 
	{"MQL", "mql"}, 
	{"Mscgen", "mscgen"}, 
	{"MuPAD", "mupad"}, 
	{"MXML", "mxml"}, 
	{"Myghty", "myghty"}, 
	{"MySQL", "mysql"}, 
	{"NASM", "nasm"}, 
	{"NCL", "ncl"}, 
	{"Nemerle", "nemerle"}, 
	{"nesC", "nesc"}, 
	{"NewLisp", "newlisp"}, 
	{"Newspeak", "newspeak"}, 
	{"Angular2", "ng2"}, 
	{"Nginx configuration file", "nginx"}, 
	{"Nimrod", "nim"}, 
	{"Nit", "nit"}, 
	{"Nix", "nixos"}, 
	{"NSIS", "nsis"}, 
	{"NumPy", "numpy"}, 
	{"NuSMV", "nusmv"}, 
	{"objdump", "objdump"}, 
	{"objdump-nasm", "objdump-nasm"}, 
	{"Objective-C", "objective-c"}, 
	{"Objective-C++", "objective-c++"}, 
	{"Objective-J", "objective-j"}, 
	{"OCaml", "ocaml"}, 
	{"Octave", "octave"}, 
	{"ODIN", "odin"}, 
	{"Ooc", "ooc"}, 
	{"Opa", "opa"}, 
	{"OpenEdge ABL", "openedge"}, 
	{"PacmanConf", "pacmanconf"}, 
	{"Pan", "pan"}, 
	{"ParaSail", "parasail"}, 
	{"Pawn", "pawn"}, 
	{"Perl", "perl"}, 
	{"Perl6", "perl6"}, 
	{"PHP", "php"}, 
	{"Pig", "pig"}, 
	{"Pike", "pike"}, 
	{"PkgConfig", "pkgconfig"}, 
	{"PL/pgSQL", "plpgsql"}, 
	{"PostgreSQL SQL dialect", "postgresql"}, 
	{"PostScript", "postscript"}, 
	{"Gettext Catalog", "pot"}, 
	{"POVRay", "pov"}, 
	{"PowerShell", "powershell"}, 
	{"Praat", "praat"}, 
	{"Prolog", "prolog"}, 
	{"Properties", "properties"}, 
	{"Protocol Buffer", "protobuf"}, 
	{"PowerShell Session", "ps1con"}, 
	{"PostgreSQL console (psql)", "psql"}, 
	{"Pug", "pug"}, 
	{"Puppet", "puppet"}, 
	{"Python 3.0 Traceback", "py3tb"}, 
	{"Python console session", "pycon"}, 
	{"PyPy Log", "pypylog"}, 
	{"Python Traceback", "pytb"}, 
	{"Python", "python"}, 
	{"Python 3", "python3"}, 
	{"QBasic", "qbasic"}, 
	{"QML", "qml"}, 
	{"QVTO", "qvto"}, 
	{"Racket", "racket"}, 
	{"Ragel", "ragel"}, 
	{"Ragel in C Host", "ragel-c"}, 
	{"Ragel in CPP Host", "ragel-cpp"}, 
	{"Ragel in D Host", "ragel-d"}, 
	{"Embedded Ragel", "ragel-em"}, 
	{"Ragel in Java Host", "ragel-java"}, 
	{"Ragel in Objective C Host", "ragel-objc"}, 
	{"Ragel in Ruby Host", "ragel-ruby"}, 
	{"Raw token data", "raw"}, 
	{"Ruby", "rb"}, 
	{"Ruby irb session", "rbcon"}, 
	{"RConsole", "rconsole"}, 
	{"Rd", "rd"}, 
	{"REBOL", "rebol"}, 
	{"Red", "red"}, 
	{"Redcode", "redcode"}, 
	{"reg", "registry"}, 
	{"ResourceBundle", "resource"}, 
	{"Rexx", "rexx"}, 
	{"RHTML", "rhtml"}, 
	{"Relax-NG Compact", "rnc"}, 
	{"Roboconf Graph", "roboconf-graph"}, 
	{"Roboconf Instances", "roboconf-instances"}, 
	{"RobotFramework", "robotframework"}, 
	{"RQL", "rql"}, 
	{"RSL", "rsl"}, 
	{"reStructuredText", "rst"}, 
	{"TrafficScript", "rts"}, 
	{"Rust", "rust"}, 
	{"SAS", "sas"}, 
	{"Sass", "sass"}, 
	{"SuperCollider", "sc"}, 
	{"Scala", "scala"}, 
	{"Scaml", "scaml"}, 
	{"Scheme", "scheme"}, 
	{"Scilab", "scilab"}, 
	{"SCSS", "scss"}, 
	{"Shen", "shen"}, 
	{"Silver", "silver"}, 
	{"Slim", "slim"}, 
	{"Smali", "smali"}, 
	{"Smalltalk", "smalltalk"}, 
	{"Smarty", "smarty"}, 
	{"Standard ML", "sml"}, 
	{"Snobol", "snobol"}, 
	{"Snowball", "snowball"}, 
	{"Debian Sourcelist", "sourceslist"}, 
	{"SourcePawn", "sp"}, 
	{"SPARQL", "sparql"}, 
	{"RPMSpec", "spec"}, 
	{"S", "splus"}, 
	{"SQL", "sql"}, 
	{"sqlite3con", "sqlite3"}, 
	{"SquidConf", "squidconf"}, 
	{"Scalate Server Page", "ssp"}, 
	{"Stan", "stan"}, 
	{"Stata", "stata"}, 
	{"Swift", "swift"}, 
	{"SWIG", "swig"}, 
	{"systemverilog", "systemverilog"}, 
	{"TADS 3", "tads3"}, 
	{"TAP", "tap"}, 
	{"TASM", "tasm"}, 
	{"Tcl", "tcl"}, 
	{"Tcsh", "tcsh"}, 
	{"Tcsh Session", "tcshcon"}, 
	{"Tea", "tea"}, 
	{"Termcap", "termcap"}, 
	{"Terminfo", "terminfo"}, 
	{"Terraform", "terraform"}, 
	{"TeX", "tex"}, 
	{"Text only", "text"}, 
	{"Thrift", "thrift"}, 
	{"Todotxt", "todotxt"}, 
	{"MoinMoin/Trac Wiki markup", "trac-wiki"}, 
	{"Treetop", "treetop"}, 
	{"TypeScript", "ts"}, 
	{"Transact-SQL", "tsql"}, 
	{"Turtle", "turtle"}, 
	{"Twig", "twig"}, 
	{"TypoScript", "typoscript"}, 
	{"TypoScriptCssData", "typoscriptcssdata"}, 
	{"TypoScriptHtmlData", "typoscripthtmldata"}, 
	{"UrbiScript", "urbiscript"}, 
	{"Vala", "vala"}, 
	{"VB.net", "vb.net"}, 
	{"VCL", "vcl"}, 
	{"VCLSnippets", "vclsnippets"}, 
	{"VCTreeStatus", "vctreestatus"}, 
	{"Velocity", "velocity"}, 
	{"verilog", "verilog"}, 
	{"VGL", "vgl"}, 
	{"vhdl", "vhdl"}, 
	{"VimL", "vim"}, 
	{"WDiff", "wdiff"}, 
	{"Whiley", "whiley"}, 
	{"X10", "x10"}, 
	{"XML", "xml"}, 
	{"XML+Cheetah", "xml+cheetah"}, 
	{"XML+Django/Jinja", "xml+django"}, 
	{"XML+Ruby", "xml+erb"}, 
	{"XML+Evoque", "xml+evoque"}, 
	{"XML+Lasso", "xml+lasso"}, 
	{"XML+Mako", "xml+mako"}, 
	{"XML+Myghty", "xml+myghty"}, 
	{"XML+PHP", "xml+php"}, 
	{"XML+Smarty", "xml+smarty"}, 
	{"XML+Velocity", "xml+velocity"}, 
	{"XQuery", "xquery"}, 
	{"XSLT", "xslt"}, 
	{"Xtend", "xtend"}, 
	{"XUL+mozpreproc", "xul+mozpreproc"}, 
	{"YAML", "yaml"}, 
	{"YAML+Jinja", "yaml+jinja"}, 
	{"Zephir", "zephir"} 
};

static const std::initializer_list <std::pair <std::string, std::string>> psExpirationPairList =
{
	{"A day", "day"}, 
	{"A week", "week"}, 
	{"A month", "month"}, 
	{"A year", "year"}
};

#endif
