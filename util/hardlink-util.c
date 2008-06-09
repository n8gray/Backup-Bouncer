char *notes = 
"   Introduced with Leopard: the ability to do directory hardlinks.  This is\n"
"   not a supported operation and the link() man page claims it doesn't work,\n"
"   but Unsanity's Rosyna Keller figured out the following set of conditions\n"
"   under which it can be done on HFS+ volumes:\n"
"        - Source parent and destination parent cannot match\n"
"        - A link is not permitted in the root directory\n"
"        - Parent of 'pointed at' directory is not the root directory\n"
"        - The 'pointed at' directory (source) is not an ancestor of the new \n"
"            directory hard link (destination).\n"
"        - No ancestor of the new directory hard link (destination) is a \n"
"            directory hard link.\n";

#include <unistd.h>
#include <stdio.h>
int
main(int argc, char *argv[])
{
   if (argc != 3) {
       fprintf(stderr, "Usage: %s <pointed at> <new link>\n\n%s"
               , argv[0], notes);
       return 1;
   }
   int ret = link(argv[1], argv[2]);
   if (ret != 0)
      perror("link");
   return ret;
}
