# Simple-Network-File-System

### How to test -

one console:
- ./serverSNFS -port your_port -mount 'servertmp'


second console:
  - ./clientSNFS -serverport your_port -serveraddress localhost -mount /freespace/local/testfuse
  - (mount dir can be named whatever but it has to be in freespace)
  - when you want to recompile/retest client run fusermount -uz /freespace/local/testfuse/ first which demounts the fuse directory
- to test readdir run ls /freespace/local/testfuse/ 




### TODO
- write implements sentences
- remove offsets from write
- switch offset before data
- vi is not gonna work
