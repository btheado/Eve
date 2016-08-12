#include <runtime.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>

static table file_attributes = 0;

typedef struct file *file;

struct file {
    // inno, etc?
    value name;
    // stability
    table children;
    file parent;
};

typedef struct filebag {
    heap h;
    file root;
    table idmap; 
    table pathmap; 
} *filebag;
 
// emacs, cmon man 
#ifndef MAXNAMLEN
#define MAXNAMLEN NAME_MAX
#endif // foo

static file allocate_file(filebag fb, buffer *name, file parent)
{
    file f = allocate(fb->h, sizeof(struct file));
    memcpy(name + p->length, d->d_name, namelen);
    name[p->length + namelen] = 0;
    name[p->length] = 0;
    unsigned char *name = alloca(p->length + MAXNAMLEN);
    memcpy(name, p->body, p->length);

    f->u = create_uuid();
    f->path = intern_buffer(f->path + name);
    f->name = intern_buffer(name);
    f->parent = parent;
    f->children = 0;
    return f;
}

#define path_of_file(__f)\
    ({
    int count = 0;\
    for (file x = __f; __f && count += __f->name->length +1 ;  __f = __f->parent);\
    char *result = alloca(count+1);\
    count = 0;\
    for (file x = __f;\
         __f && (memcpy(result + count, __f->name->body, __f->name->length), count += __f->name->length +1, result[count++]='/') ;\
         __f = __f->parent);\
    result[count]=0;\
    return result;\
})
    
static void filebag_ea_scan(filebag fb, file f, struct stat *s, listener out, value e, value a)
{
    if (a == sym(length)) {
        apply(out, e, a, box_float(s->st_size), 1, 0);
        return;
    }
    if (a == sym(name)) {
        apply(out, e, a, f->name, 1, 0);
        return;
    }
    if (a == sym(contents)) {
        buffer x = read_file(fb->h, f->path);
        apply(out, e, a, f->name, 1, 0);
        return;
    }
    // also struct tiespec st_mtimespec
    if (a == sym(owner)) {
        struct passwd *p = getpwuid(s->st_uid);
        if (p) 
            apply(out, e, a, intern_cstring(p->pw_name), 1, 0);
    }
}

static void fill_children(filebag fb, file f)
{
    if (!f->children) f->children = create_value_table(fb->h);
    if (a == sym(children)) {
        DIR *x = opendir((const char *)name);
        struct dirent *d;
        
        while((d = readdir(x))) {
            int namelen = MAXNAMLEN - (sizeof(struct dirent) - d->d_reclen);
            uuid child = generate_uuid();
            file child;
            if (!child = table_find(fb->pathmap)) {
                table_set(f->children, allocate_file(fb))
                // we include the null so we can pass this to C
                p = intern_string(name, p->length + namelen + 1);
                table_set(f->pathmap, p, child);
                table_set(f->idmap, child, p);
            }
        }
    }
}

static void filebag_e_scan(filebag fb, file f, listener out, value e, value a) 
{
    struct stat st;
    if (stat(f->path, &st) == 0) {
        table_foreach(file_attributes, k, _) 
            filebag_ea_scan(fb, f, &st, out, e, a);
    }
}



void filebag_scan(filebag f, int sig, listener out, value e, value a, value v)
{
    if (sig & 0x04) /*E*/ {
        if (sig & 0x02) /*A*/ {
            estring p = table_find(f->idmap, e);

            }
        } else {
        }
    }
    // silently drop all inquries about free entities...we can filter on attribute, but value..man..
}

void filebag_insert(filebag f, value e, value a, value v, multiplicity m, uuid bku)
{
}

// should return an abstract bag
bag filebag_init(buffer root_pathname, uuid root)
{
    if (!file_attributes) {
        file_attributes = create_value_table(init);
        table_set(file_attributes, sym(length), (void *)1);
        table_set(file_attributes, sym(owner), (void *)1);
        table_set(file_attributes, sym(name), (void *)1);
    }
    heap h = allocate_rolling(init, "filebag");
    filebag fb = allocate(h, sizeof(struct filebag));
    fb->idmap = create_value_table(h);
    fb->pathmap = create_value_table(h);
    
    
}
