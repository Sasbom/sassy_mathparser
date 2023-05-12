## Sassy math parser

As an exercise I made  math sentence parser. It does not use conventional methods,
as you can clearly see from the code. This is all a thought experiment and it was pretty interesting! <br><br>


Instead of normal methods of parsing a math string, it parses everything into an N-tree and evaluates it kind of like a human would.
Play around with it at your own caution, this is not guarranteed to produce biblically accurate results!
<br><br>
The way it <i>tries</i> to go about its business is by:
- first seperating all parentheses into sub expression tokens
- then tokenizing string sub expressions into  operands and values next to the expressions
- inserting "implied" multiply operands where conditions are right

known limits:
- can not do -(someexpression)
- sometimes it'll fuck up order of operations compared to google search engine calculator
<br>
known good things:
- it can do those stupid trivial math memes ur granma posts on facebook
- it can do the thing where it detects 2 subexpressions like (one)(two)(bucklemyshoe) and insert *'s
- it prints a cool and epic output 
- really basic error reporting
<br>
Have fun!!