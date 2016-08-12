#include <runtime.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>

static table file_attributes = 0;


typedef struct file {
    // inno, etc?
    value name;
    // this could be derivable
    char *path;
    // stability
    table children;
} *file;

typedef struct filebag {
    heap h;
    file root;
} *filebag;
 
// emacs, cmon man 
#ifndef MAXNAMLEN
#define MAXNAMLEN NAME_MAX
#endif // foo


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
    if (sig & 0x04) {
        if (sig & 0x02) {
            estring p = table_find(f->idmap, e);
            char *name = alloca(p->length + MAXNAMELEN);
            memcpy(name, p->body, p->length);
            if (a == sym(children)) {
                name[p->length] = 0;
                DIR *x = opendir(name);
                struct dirent *f;
                
                while((f = readdir(x))) {
                    int namelen = MAXNAMELEN - (sizeof(struct dirent) - f->d_reclen);
                    child = generate_uuid();
                    memcpy(name + p->length, f->d_name, namelen);
                    
                    table_set(f->idmap, child, intern_string(name ,p->length + namelen));
                    edb_insert(b, f, sym(children), child);
                }
            }
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
    
}
