# Nasal Interpreter

# Nasal script language

[Nasal](http://wiki.flightgear.org/Nasal_scripting_language) is a script language that used in [FlightGear](https://www.flightgear.org/).

There is a Nasal console in FlightGear but sometimes it is not so easy for every developer to use.

So this is an interpreter for Nasal written by C++.

The interpreter is still in development.Anyone who interested in this could also join us!
  
# Lexical Analysis
  
What do it's outputs look like?
  
> ( identifier   | engineTimer )

> ( operator     | . )

> ( identifier   | start )

> ( operator     | ; )

> ( reserve word | print )

> ( operator     | ( )

> ( string       | Engine started )

> ( operator     | ) )

> ( operator     | ; )

This is what it outputs.

# Push down automata

After many times of failure,i finally try to use PDA to do the parse work.

But something occurred,so i finally didn't make it.However you can still see this LL(1) and LR(0) parser in __version 0.7 & 0.9__ .

# Parser

The parser can recognize some basic elements in resource program.

In __version 1.1__ you can use the fully-functional parser.

But there are still some differences such as:

```javascript
(var a,b,c)=(1,2,3);
var (r,g,b)=color;
(a,b)=(b,a);
```

etc. cannot be recognized.

And each statement must have a ';' after it or you will be informed with __'error: expect a ';''__ .

# Calculator

You can try a calculator in __version 0.17~0.19__ !

# Abstract Syntax Tree

In __version 1.2__ the ast has been completed.

But there are still some bugs inside.

# Balloon script

[Balloon](https://github.com/ValKmjolnir/Balloon-script) is created for experiment.

It is a subset of Nasal and the parser is __stricter__ than Nasal parser.

Explore the usage of balloon and enjoy yourself! XD
