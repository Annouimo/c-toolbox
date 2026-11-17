/* SPDX-License-Identifier: MIT-0 */
static StringList read_words(size_t count){StringList result={0};for(size_t i=0;i<count;++i){String s=read_word();string_list_push_take(&result,&s);}return result;}
