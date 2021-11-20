.\" PS PE macro test to include plot files in troff documents.
.LP
This is just some stuff to see whether I can put a picture into a document.
I plan to do this by first looking at the output that troff generates for
this document, and then intuiting the structure of the output stream.  This
should tell me how to recognize the beginning of the picture and how to
have the filter suck in the text of the picture as it gets printed.  I
hope that the damn thing works, after all that.
.LP
My first test will to be to have a stupid picture which will appear in
the space below.
.DS L
.rs
.sp 4i
\(ct
.DE
This should be some text following the picture.  Notice how it didn't work
at all?  This little project should keep me busy for a great deal of time
tonight, and probably for the next three days or so.
.LP
If it winds up working, I'll be a monkey's uncle.
