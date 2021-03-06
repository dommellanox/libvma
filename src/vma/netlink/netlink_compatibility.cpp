/*
 * Copyright (C) Mellanox Technologies Ltd. 2001-2013.  ALL RIGHTS RESERVED.
 *
 * This software product is a proprietary product of Mellanox Technologies Ltd.
 * (the "Company") and all right, title, and interest in and to the software product,
 * including all associated intellectual property rights, are and shall
 * remain exclusively with the Company.
 *
 * This software is made available under either the GPL v2 license or a commercial license.
 * If you wish to obtain a commercial license, please contact Mellanox at support@mellanox.com.
 */

#include "netlink_compatibility.h"
#include "vma/util/bullseye.h"
#include "vma/util/if.h"
#include "vlogger/vlogger.h"

#define MODULE_NAME 		"nl_wrapper:"
#define nl_logerr		__log_err
#define nl_logwarn		__log_warn


extern void link_event_callback(nl_object* obj);
extern void neigh_event_callback(nl_object* obj);
extern void route_event_callback(nl_object* obj);

#ifdef HAVE_LIBNL3

nl_sock* nl_socket_handle_alloc() {
	return nl_socket_alloc();
}

void nl_socket_handle_free(struct nl_sock * sock) {
	nl_socket_free(sock);
}

void neigh_callback(nl_cache* , nl_object* obj, int, void*) {
	neigh_event_callback(obj);
}

void link_callback(nl_cache* , nl_object* obj, int, void*) {
	link_event_callback(obj);
}

void route_callback(nl_cache* , nl_object* obj, int, void*) {
	route_event_callback(obj);
}

void nl_socket_handle_disable_seq_check(nl_socket_handle* handle) {
	return nl_socket_disable_seq_check(handle);
}

nl_cache_mngr* nl_cache_mngr_compatible_alloc(nl_socket_handle* handle, int protocol, int flags) {
	nl_cache_mngr* cache_mngr;

	/* allocate temporary 10 nl_sockets for marking the first 10 bits of user_port_map[0] (@[libnl/lib/socket.c]) as workaround
	 * to avoid conflict between the cache manager's internal sync socket and other netlink sockets on same process
	 */
	struct nl_sock* tmp_socket_arr[10];
	for (int i=0; i<10; i++) {
		tmp_socket_arr[i] = nl_socket_handle_alloc();
	}

	int err = nl_cache_mngr_alloc(handle, protocol, flags, &cache_mngr);

	// free the temporary sockets after cache manager was allocated and bounded the sync socket
	for (int i=0; i<10; i++) {
		nl_socket_free(tmp_socket_arr[i]);
	}

	BULLSEYE_EXCLUDE_BLOCK_START
	if (err) {
		nl_logerr("Fail to allocate cache manager, error=%s", nl_geterror(err));
		return NULL;
	}
	int nl_socket_fd = nl_socket_get_fd(handle);
	if (fcntl(nl_socket_fd, F_SETFD, FD_CLOEXEC) != 0) {
		nl_logwarn("Fail in fctl, error = %d", errno);
	}
	BULLSEYE_EXCLUDE_BLOCK_END

	return cache_mngr;
}

int nl_cache_mngr_compatible_add(struct nl_cache_mngr*	mngr, const char* name, change_func_t cb, void*	data, struct nl_cache** result){
	int err = nl_cache_mngr_add(mngr, name, cb, data, result);
	BULLSEYE_EXCLUDE_BLOCK_START
	if (err) {
		nl_logerr("Fail to add to cache manager, error=%s", nl_geterror(err));
	}
	BULLSEYE_EXCLUDE_BLOCK_END
	return err;
}

int nl_object_get_compatible_msgtype(const struct nl_object* obj) {
	return nl_object_get_msgtype(obj);
}

const char*	get_rtnl_route_iif_name(struct rtnl_route* route) {
	char iifstr[IFNAMSIZ];
	return if_indextoname(rtnl_route_get_iif(route), iifstr);
}


#else //HAVE_LIBNL1

// structure to pass arguments on internal netlink callbacks handling
typedef struct rcv_msg_arg
{
	netlink_wrapper* netlink;
	struct nl_handle* socket_handle;
	map<e_netlink_event_type, subject*>* subjects_map;
	nlmsghdr* msghdr;
} rcv_msg_arg_t;

nl_handle* nl_socket_handle_alloc() {
	return nl_handle_alloc();
}

void nl_socket_handle_free(struct nl_handle* handle) {
	nl_handle_destroy(handle);
}

void neigh_callback(nl_cache* , nl_object* obj, int) {
	neigh_event_callback(obj);
}

void link_callback(nl_cache* , nl_object* obj, int) {
	link_event_callback(obj);
}

void route_callback(nl_cache* , nl_object* obj, int) {
	route_event_callback(obj);
}

void nl_socket_handle_disable_seq_check(nl_socket_handle* handle) {
	return nl_disable_sequence_check(handle);
}

nl_cache_mngr* nl_cache_mngr_compatible_alloc(nl_socket_handle* handle, int protocol, int flags) {
	nl_cache_mngr* cache_mgr = nl_cache_mngr_alloc(handle, protocol, flags);

	BULLSEYE_EXCLUDE_BLOCK_START
	if (!cache_mgr) {
		nl_logerr("Fail to allocate cache manager");
	}
	
	int nl_socket_fd = nl_socket_get_fd(handle);
	if (fcntl(nl_socket_fd, F_SETFD, FD_CLOEXEC) != 0) {
		nl_logwarn("Fail in fctl, error = %d", errno);
	}
	BULLSEYE_EXCLUDE_BLOCK_END

	return cache_mgr;
}

int nl_cache_mngr_compatible_add(struct nl_cache_mngr*	mngr, const char* name, change_func_t cb, void*	, struct nl_cache** result){
	*result = nl_cache_mngr_add(mngr, name, cb);
	return 0;
}

int nl_object_get_compatible_msgtype(const struct nl_object* obj) {
	_nl_object* _obj = (_nl_object*)obj;
	return _obj->ce_msgtype;
}

const char*	get_rtnl_route_iif_name(struct rtnl_route* route) {
	return rtnl_route_get_iif(route);
}

#endif
