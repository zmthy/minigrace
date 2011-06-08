#include "unicodedata.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct Method {
    char *name;
    struct Object* (*func)(struct Object*, int,
            struct Object**);
    struct Object*** closure;
    struct Object* (*cfunc)(struct Object*, int,
            struct Object**, struct Object ***closure);
};

struct Object {
#ifndef NO_FLAGS
    int32_t flags;
#endif
    char type[32];
    struct Method **methods;
    struct Object **data;
    void **bdata;
    int nummethods;
    int methodspace;
};

void addmethod(struct Object*, char*,
        struct Object* (*)(struct Object*, int, struct Object**));
struct Object *Float64_asString(struct Object*, int nparams,
        struct Object**);
struct Object *alloc_Float64(double);
struct Object *Float64_Add(struct Object*, int nparams,
        struct Object**);
struct Object *Object_asString(struct Object*, int nparams,
        struct Object**);
struct Object *Object_concat(struct Object*, int nparams,
        struct Object**);
struct Object *Object_NotEquals(struct Object*, int,
        struct Object**);
struct Object *Object_Equals(struct Object*, int,
        struct Object**);
struct Object* alloc_String(const char*);
struct Object *String_concat(struct Object*, int nparams,
        struct Object**);
struct Object *String_index(struct Object*, int nparams,
        struct Object**);
struct Object *callmethod(struct Object *receiver, const char *name,
        int nparams, struct Object **args);
struct Object *alloc_Boolean(int val);
struct Object *alloc_Octets(const char *data, int len);
struct Object *alloc_obj();
char *cstringfromString(struct Object*);
int integerfromAny(struct Object*);

struct Object *unicode_module = NULL;

struct Object *unicode_name(struct Object *self, int nparams,
         struct Object **args) {
    struct Object *o = callmethod(args[0], "ord", 0, 0);
    int v = integerfromAny(o);
    const char *name = Unicode_Names[v];
    return alloc_String(name);
}
struct Object *unicode_category(struct Object *self, int nparams,
         struct Object **args) {
    struct Object *o = callmethod(args[0], "ord", 0, 0);
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    return alloc_String(Unicode_Categories[cindex]);
}
struct Object *unicode_combining(struct Object *self, int nparams,
         struct Object **args) {
    struct Object *o = callmethod(args[0], "ord", 0, 0);
    int v = integerfromAny(o);
    int cval = UnicodeRecords[v].combining;
    return alloc_Float64(cval);
}
struct Object *unicode_mirrored(struct Object *self, int nparams,
         struct Object **args) {
    struct Object *o = callmethod(args[0], "ord", 0, 0);
    int v = integerfromAny(o);
    int cval = UnicodeRecords[v].mirrored;
    return alloc_Boolean(cval == 'Y');
}
struct Object *unicode_bidirectional(struct Object *self, int nparams,
         struct Object **args) {
    struct Object *o = callmethod(args[0], "ord", 0, 0);
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].bidirectional;
    return alloc_String(Unicode_Bidirectionals[cindex]);
}
struct Object *unicode_iscategory(struct Object *self, int nparams,
        struct Object **args) {
    struct Object *o = callmethod(args[0], "ord", 0, 0);
    struct Object *co = args[1];
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    const char *cat = Unicode_Categories[cindex];
    char *dt = cstringfromString(co);
    struct Object *ret = alloc_Boolean(0);
    if (strlen(dt) == 1) {
        if (cat[0] == dt[0])
            ret = alloc_Boolean(1);
    }
    if (strcmp(cat, dt) == 0)
        ret = alloc_Boolean(1);
    free(dt);
    return ret;
}
struct Object *unicode_isSeparator(struct Object *self, int nparams,
        struct Object **args) {
    struct Object *o = args[0];
    if (strcmp(args[0]->type, "String") == 0) {
        o = callmethod(args[0], "ord", 0, 0);
    }
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    const char *cat = Unicode_Categories[cindex];
    return alloc_Boolean(cat[0] == 'Z');
}
struct Object *unicode_isControl(struct Object *self, int nparams,
        struct Object **args) {
    struct Object *o = args[0];
    if (strcmp(args[0]->type, "String") == 0) {
        o = callmethod(args[0], "ord", 0, 0);
    }
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    const char *cat = Unicode_Categories[cindex];
    return alloc_Boolean(cat[0] == 'C');
}
struct Object *unicode_isLetter(struct Object *self, int nparams,
        struct Object **args) {
    struct Object *o = args[0];
    if (strcmp(args[0]->type, "String") == 0) {
        o = callmethod(args[0], "ord", 0, 0);
    }
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    const char *cat = Unicode_Categories[cindex];
    return alloc_Boolean(cat[0] == 'L');
}
struct Object *unicode_isNumber(struct Object *self, int nparams,
        struct Object **args) {
    struct Object *o = args[0];
    if (strcmp(args[0]->type, "String") == 0) {
        o = callmethod(args[0], "ord", 0, 0);
    }
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    const char *cat = Unicode_Categories[cindex];
    return alloc_Boolean(cat[0] == 'N');
}
struct Object *unicode_isSymbolMathematical(struct Object *self, int nparams,
        struct Object **args) {
    struct Object *o = args[0];
    if (strcmp(args[0]->type, "String") == 0) {
        o = callmethod(args[0], "ord", 0, 0);
    }
    int v = integerfromAny(o);
    int cindex = UnicodeRecords[v].category;
    const char *cat = Unicode_Categories[cindex];
    return alloc_Boolean(strcmp(cat, "Sm") == 0);
}
struct Object *unicode_create(struct Object *self, int nparams,
         struct Object **args) {
    struct Object *o = args[0];
    int cp = integerfromAny(o);
    char buf[5];
    int i;
    int tmp;
    for (i=0; i<5; i++)
        buf[i] = 0;
    if (cp < 128)
         buf[0] = cp;
    else if (cp < 0x0800) {
        buf[0] = 0xc0;
        buf[1] = 0x80;
        tmp = cp >> 6;
        tmp &= 0x1f;
        buf[0] |= tmp;
        tmp = cp & 0x3f;
        buf[1] |= tmp;
    } else if (cp < 0x10000) {
        buf[0] = 0xe0;
        buf[1] = 0x80;
        buf[2] = 0x80;
        tmp = cp >> 12;
        tmp &= 0xf;
        buf[0] |= tmp;
        tmp = cp >> 6;
        tmp &= 0x1f;
        buf[1] |= tmp;
        tmp = cp & 0x3f;
        buf[2] |= tmp;
    } else if (cp < 0x10FFFF) {
        buf[0] = 0xf0;
        buf[1] = 0x80;
        buf[2] = 0x80;
        buf[3] = 0x80;
        tmp = cp >> 16;
        tmp &= 0x8;
        buf[0] |= tmp;
        tmp = cp >> 12;
        tmp &= 0x1f;
        buf[1] |= tmp;
        tmp = cp >> 6;
        tmp &= 0x1f;
        buf[2] |= tmp;
        tmp = cp & 0x3f;
        buf[3] |= tmp;
    }
    return alloc_String((char*)buf);
}
struct Object *module_unicode_init() {
    if (unicode_module != NULL)
        return unicode_module;
    struct Object *o = alloc_obj();
    addmethod(o, "category", &unicode_category);
    addmethod(o, "bidirectional", &unicode_bidirectional);
    addmethod(o, "combining", &unicode_combining);
    addmethod(o, "mirrored", &unicode_mirrored);
    addmethod(o, "name", &unicode_name);
    addmethod(o, "create", &unicode_create);
    addmethod(o, "iscategory", &unicode_iscategory);
    addmethod(o, "isSeparator", &unicode_isSeparator);
    addmethod(o, "isControl", &unicode_isControl);
    addmethod(o, "isLetter", &unicode_isLetter);
    addmethod(o, "isNumber", &unicode_isNumber);
    addmethod(o, "isSymbolMathematical", &unicode_isSymbolMathematical);
    unicode_module = o;
    return o;
}
