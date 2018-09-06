#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iolayer.h"
#include "aux.h"

struct generic_record_t simulate_keylist_record(struct PDirectory* pdir) {
    struct generic_record_t record;

    return record;
}

void simulate_streamer_record(struct llio_t *llio) {
    // init
    llio->streamer_record.blob = NULL;

    // simulate blob
    uint32_t n_sinfo = 0u;
    int16_t version_sinfo = 5;
    struct PObject obj;
    obj.version = 2;
    obj.id = 0x1111;
    obj.bits = 0x1111;
    struct PString str_sinfo;
    str_sinfo.size = 0;
    uint32_t nbytes_sinfo = size_object(&obj) + 
                            size_in_bytes_string(&str_sinfo) +
                            2 + 4;

    // note: we do not free this resource.
    // TODO
    char *buffer_sinfo = malloc(nbytes_sinfo);
    char *tmp_sinfo = buffer_sinfo;
    put_version(&buffer_sinfo, version_sinfo);
    to_buf_object(&buffer_sinfo, &obj);
    to_buf_pstring(&buffer_sinfo, &str_sinfo);
    put_u32(&buffer_sinfo, n_sinfo);
    llio->streamer_record.blob = tmp_sinfo;
    
    // generate a key for that blob
    struct PString class_name, obj_name, title_name;
    char *streamer = "StreamerInfo";
    char *tlist = "Doubly linked list";
    ctor_nomemcopy_pstring(&class_name, "TList", 5);
    ctor_nomemcopy_pstring(&obj_name, streamer, strlen(streamer));
    ctor_nomemcopy_pstring(&title_name, tlist, strlen(tlist));
    ctor_withnames_key(&llio->streamer_record.key, &class_name, &obj_name, &title_name);
    llio->streamer_record.key.seek_pdir = llio->header.begin;
    llio->streamer_record.key.key_bytes = size_key(&llio->streamer_record.key);
    llio->streamer_record.key.obj_bytes = nbytes_sinfo;
    llio->streamer_record.key.total_bytes = llio->streamer_record.key.obj_bytes +
        llio->streamer_record.key.key_bytes;

    // key and blob are ready
}

void simulate_free_segments_record(struct llio_t *llio) {
    // simulate 0 free segments 
    llio->free_segments_record.pfree = NULL;
    llio->free_segments_record.length = 0;

    // generate the key for that list
    struct PString class_name;
    ctor_nomemcopy_pstring(&class_name, "TFile", 5);
    ctor_withnames_key(&llio->free_segments_record.key, &class_name, 
                       &llio->top_dir_rec.named.name, 
                       &llio->top_dir_rec.named.title);
    llio->free_segments_record.key.seek_pdir = llio->header.begin;
    llio->free_segments_record.key.key_bytes = size_key(&llio->free_segments_record.key);
    llio->free_segments_record.key.obj_bytes = 0;
    llio->free_segments_record.key.total_bytes = llio->free_segments_record.key.key_bytes;

    // key and free list are ready
}

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct llio_t llio = open_to_write(filename);

    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes);

    // we simulate the streamer and free segments records
    simulate_streamer_record(&llio);
    simulate_free_segments_record(&llio);

    printf("--- simulated streamer and free segments records ---\n");
    print_key(&llio.streamer_record.key);
    print_key(&llio.free_segments_record.key);

    // sequence of actions to do upon closing the file
    write_streamer_record(&llio);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes + 
           llio.streamer_record.key.total_bytes);

    write_free_segments_record(&llio);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes + 
           llio.streamer_record.key.total_bytes + 
           llio.free_segments_record.key.total_bytes);

    write_end_byte(&llio);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes + 
           llio.streamer_record.key.total_bytes + 
           llio.free_segments_record.key.total_bytes + 1);

    write_top_dir_record(&llio);
    write_file_header(&llio);
    close_from_write(&llio);

    return 0;
}
