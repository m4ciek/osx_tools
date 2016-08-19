/*
clear-acl.c: wipe ACLs from a symlink in Mac OS X
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/acl.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

	if(argc<2) {
		fprintf(stderr, "usage: %s file\n", argv[0]);
		return 1;
	}

	acl_t acl_for_symlink;

	if ((acl_t)NULL == (acl_for_symlink = acl_get_link_np(argv[1], ACL_TYPE_EXTENDED))) {
		int try_open_fd;

		if(
			-1==(try_open_fd=open(argv[1], O_SYMLINK)) ||
			-1==close(try_open_fd) ||
			-1==readlink(argv[1],NULL,0)
		)
			perror(argv[0]);
		else
			fprintf(stderr,"%s has no ACLs\n",argv[1]);
		
		return 1;
	}

	int ret_acl_get_entry;
	acl_entry_t an_entry;

	for(
		ret_acl_get_entry = acl_get_entry(acl_for_symlink, ACL_FIRST_ENTRY, &an_entry)
		; -1 != ret_acl_get_entry ;
		ret_acl_get_entry = acl_get_entry(acl_for_symlink, ACL_NEXT_ENTRY, &an_entry)
	) if(-1==(acl_delete_entry(acl_for_symlink, an_entry))) {
		perror(argv[0]);
		return 1;
	}

	int symlink_fd;

	if(
		-1==(symlink_fd=open(argv[1], O_SYMLINK)) ||
		-1==acl_set_fd_np(symlink_fd, acl_for_symlink, ACL_TYPE_EXTENDED) ||
		-1==close(symlink_fd) ||
		-1==acl_free(acl_for_symlink)
	) {
		perror(argv[0]);
		return 1;
	}

	return 0;
}
