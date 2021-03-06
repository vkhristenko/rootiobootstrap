#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rootio/core/localfs.h"

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    // open for a read using local filesystem api
    struct localfs_file_context_t ctx = localfs_open_to_read(filename);

    // transfer a file header to memory
    localfs_read_file_header(&ctx);
    print_file_header(&ctx.structure.header);

    printf("--- ---- ---\n");

    // transfer a streamer record to memory
    struct streamer_record_t streamer_record = localfs_read_streamer_record(&ctx);
    print_key(&streamer_record.key);

    // transfer a free segments record to memory
    struct free_segments_record_t free_segments_record = 
        localfs_read_free_segments_record(&ctx);
    print_key(&free_segments_record.key);
    printf("nfree = %d\n", free_segments_record.length);
    for (int i=0; i<free_segments_record.length; i++)
        print_pfree(&(free_segments_record.pfree[i]));

    printf("--- ---- ---\n");

    localfs_read_top_dir_record(&ctx);
    print_key(&ctx.structure.top_dir_rec.key);
    printf("--- ---- ---\n");
    print_named(&ctx.structure.top_dir_rec.named);
    printf("--- ---- ---\n");
    print_dir(&ctx.structure.top_dir_rec.dir);
    printf("--- ---- ---\n");

    localfs_close_from_read(&ctx);

    return 0;
}
