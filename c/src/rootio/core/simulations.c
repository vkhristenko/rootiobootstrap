#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rootio/core/aux.h"
#include "rootio/core/simulations.h"

void simulate_streamer_record(struct localfs_file_context_t *ctx) {
    // init
    ctx->structure.streamer_record.blob = NULL;

    // simulate blob
    uint32_t n_sinfo = 0u;
    int16_t version_sinfo = 5;
    struct object_t obj;
    obj.version = 2;
    obj.id = 0x1111;
    obj.bits = 0x1111;
    struct string_t str_sinfo;
    str_sinfo.size = 0;
    int byte_count = 0;
    uint32_t nbytes_sinfo = size_object(&obj) + 
                            size_in_bytes_string(&str_sinfo) +
                            2 + 4 + 4;

    // note: we do not free this resource.
    // TODO
    char *buffer_sinfo = malloc(nbytes_sinfo);
    char *tmp_sinfo = buffer_sinfo;
    put_version(&buffer_sinfo, version_sinfo);
    to_buf_object(&buffer_sinfo, &obj);
    to_buf_pstring(&buffer_sinfo, &str_sinfo);
    put_u32(&buffer_sinfo, n_sinfo);
    put_u32(&buffer_sinfo, byte_count);
    ctx->structure.streamer_record.blob = tmp_sinfo;
    
    // generate a key for that blob
    struct string_t class_name, obj_name, title_name;
    char *streamer = "StreamerInfo";
    char *tlist = "Doubly linked list";
    ctor_nomemcopy_pstring(&class_name, "TList", 5);
    ctor_nomemcopy_pstring(&obj_name, streamer, strlen(streamer));
    ctor_nomemcopy_pstring(&title_name, tlist, strlen(tlist));
    ctor_withnames_key(&ctx->structure.streamer_record.key, &class_name, &obj_name, &title_name);
    ctx->structure.streamer_record.key.seek_pdir = ctx->structure.header.begin;
    ctx->structure.streamer_record.key.key_bytes = size_key(&ctx->structure.streamer_record.key);
    ctx->structure.streamer_record.key.obj_bytes = nbytes_sinfo;
    ctx->structure.streamer_record.key.total_bytes = ctx->structure.streamer_record.key.obj_bytes +
        ctx->structure.streamer_record.key.key_bytes;

    // key and blob are ready
}

void simulate_free_segments_record(struct localfs_file_context_t *ctx) {
    // simulate N free segments 
    int nfree = 1;
    ctx->structure.free_segments_record.length = nfree;
    ctx->structure.free_segments_record.pfree = malloc(sizeof(struct free_t) * 
        nfree);
    ctor_pfree(ctx->structure.free_segments_record.pfree, 1000, 2000000000);

    // generate the key for that list
    struct string_t class_name;
    ctor_nomemcopy_pstring(&class_name, "TFile", 5);
    ctor_withnames_key(&ctx->structure.free_segments_record.key, &class_name, 
                       &ctx->structure.top_dir_rec.named.name, 
                       &ctx->structure.top_dir_rec.named.title);
    ctx->structure.free_segments_record.key.seek_pdir = ctx->structure.header.begin;
    ctx->structure.free_segments_record.key.key_bytes = size_key(&ctx->structure.free_segments_record.key);
    ctx->structure.free_segments_record.key.obj_bytes = size_pfree(ctx->structure.free_segments_record.pfree);
    ctx->structure.free_segments_record.key.total_bytes = ctx->structure.free_segments_record.key.key_bytes + 
        ctx->structure.free_segments_record.key.obj_bytes;

    // key and free list are ready
}
