/*
 * hashes.c
 * 
 * Copyright 2021 Luis "Laffin" Espinoza <laffintoo at gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "symbols.c"


char *strupr(char *str)
{
   static char ret[81];
   int pos=0;
   while(str[pos])
      ret[pos]=toupper(str[pos++]);
   ret[pos]=0;
   return ret;
}
int main(int argc, char **argv)
{
   FILE *fh;
   st_symbols *sym;
   char line[81], *key, *val,ch;
   int len,cnt;

   fh=fopen("qemu-run.defaults","r");
   cnt=0;
   while(fgets(line,81,fh)) {
      len=strlen(line);
      while(len>0 && ((ch=line[len-1])=='\n' || ch=='\r' || ch==' ' || ch=='\t'))  --len;
      line[len]=0;
      if(!len) continue;
      key=strtok(line,"=");
      val=strtok(NULL,"=");
      printf("%s='%s'\n",key,val);
      sym_add(key,val);
   }
   fclose(fh);
   fh=fopen("config.h","w");
   fprintf(fh,"enum { ");
   for(sym=sym_first(),cnt=0;sym;cnt++,sym=sym_next()) {
      fprintf(fh,"KEY_%s,",strupr(sym->key));
   }
   fprintf(fh,"KEY_ENDLIST };\n\ntypedef struct {\n\tint hash;\n\tchar *val;\n} st_config;\n\nst_config cfg[%d] = {\n",cnt+1);
   
   for(sym=sym_first(),cnt=0;sym;cnt++,sym=sym_next())
      fprintf(fh,"\t{0x%8.8X, \"%s\"}%s\t\t/* %s */\n",sym->hash,sym->val?sym->val:"",sym->next?",":"",sym->key);
   fprintf(fh,"};");
   fclose(fh);
   return 0;
}
