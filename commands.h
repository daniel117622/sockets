#include <stdio.h>
#include <string.h>
#include <ctype.h>

void parse_command(char * buffer)
{
    // On an ocurrence of "/<some_text>\n" do something
    char * location = strchr(buffer,'/') + 1;
    // From location iterate until /n char is found
    if (location != NULL) // The '/' was found
    {
        printf("Found a command at %p with value %c! \n",location,*location);
        char * tmp = location;
        int cmd_length = 1; int i = 0;
        while ( *location != '\n' ||  *location != '\0' || i <= 100) 
        {
            if (isspace(*location))
            {
                cmd_length = i;
            }
            i++;
            location++;
        }
    }

}