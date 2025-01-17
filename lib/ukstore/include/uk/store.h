/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Craciunoiu Cezar <cezar.craciunoiu@gmail.com>
 *
 * Copyright (c) 2021, University Politehnica of Bucharest. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __UK_STORE_H__
#define __UK_STORE_H__

#include <string.h>
#include <uk/config.h>
#include <uk/assert.h>
#include <uk/list.h>
#include <uk/arch/atomic.h>
#include <uk/essentials.h>
#include <uk/alloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !CONFIG_LIBUKSTORE
#define uk_store_libid(libname) (-1)

enum uk_store_entry_type;
struct uk_store_entry;

#define __UK_STORE_STATIC_ENTRY(entry, lib_str, e_type, e_get, e_set)	\
	static const struct uk_store_entry				\
	__unused __align8						\
	__uk_store_entries_list ## _ ## entry = {			\
		.name = STRINGIFY(entry),				\
		.type       = (uk_store_get_entry_type(e_type)),	\
		.get.e_type = (e_get),					\
		.set.e_type = (e_set),					\
		.flags      = UK_STORE_ENTRY_FLAG_STATIC		\
	}

/* Do not call directly */
#define _UK_STORE_STATIC_ENTRY(entry, lib_str, e_type, e_get, e_set)	\
	__UK_STORE_STATIC_ENTRY(entry, lib_str, e_type, e_get, e_set)

/**
 * Adds an entry to the entry section of a library.
 *
 * @param entry the entry in the section
 * @param e_type the type of the function
 * @param e_get getter pointer
 * @param e_set setter pointer
 */
#define UK_STORE_STATIC_ENTRY(entry, e_type, e_get, e_set)	\
	_UK_STORE_STATIC_ENTRY(entry, STRINGIFY(__LIBNAME__),	\
		e_type, e_get, e_set)

/* Flags if an entry is static or dynamic */
#define UK_STORE_ENTRY_FLAG_STATIC	(1 << 0)

#define UK_STORE_ENTRY_ISSTATIC(entry)	\
	((entry)->flags & UK_STORE_ENTRY_FLAG_STATIC)

#define uk_store_get_folder(library_id, folder_name)	\
	do { } while (0)

#define uk_store_get_entry(libname, foldername, entryname)	\
	do { } while (0)

#define uk_store_get_value(entry, outtype, out)	\
	do { } while (0)

#define uk_store_set_value(entry, intype, in)	\
	do { } while (0)

#else /* !CONFIG_LIBUKSTORE */

/* Library-specific information generated at compile-time */
#include <uk/_store_libs.h>

/* Do not call directly */
#define _uk_store_libid(libname)	\
	__UK_STORE_ ## libname

/**
 * Returns the id of a given library.
 *
 * @param libname the library to search for (not a string, e.g. LIBVFSCORE)
 * @return the id
 */
#define uk_store_libid(libname)	\
	_uk_store_libid(libname)

#endif /* CONFIG_LIBUKSTORE */

/**
 * Returns the id of the place it was called from.
 *
 * @return this libraries' id
 */
#define uk_store_libid_self()	\
	uk_store_libid(__LIBNAME__)

/* All basic types that exist - use these when interacting with the API */
enum uk_store_entry_type {
	_LIB_UK_STORE___undef = 0,
	_LIB_UK_STORE___s8    = 1,
	_LIB_UK_STORE___u8    = 2,
	_LIB_UK_STORE___s16   = 3,
	_LIB_UK_STORE___u16   = 4,
	_LIB_UK_STORE___s32   = 5,
	_LIB_UK_STORE___u32   = 6,
	_LIB_UK_STORE___s64   = 7,
	_LIB_UK_STORE___u64   = 8,
	_LIB_UK_STORE___uptr  = 9,
	_LIB_UK_STORE___charp = 10
};

/* Transforms a simple data type to a ukstore specific one */
#define uk_store_get_entry_type(simple_type)	\
	(_LIB_UK_STORE___ ## simple_type)

/* Getter definitions */
typedef int (*uk_store_get_s8_func_t)(void *, __s8 *);
typedef int (*uk_store_get_u8_func_t)(void *, __u8 *);
typedef int (*uk_store_get_s16_func_t)(void *, __s16 *);
typedef int (*uk_store_get_u16_func_t)(void *, __u16 *);
typedef int (*uk_store_get_s32_func_t)(void *, __s32 *);
typedef int (*uk_store_get_u32_func_t)(void *, __u32 *);
typedef int (*uk_store_get_s64_func_t)(void *, __s64 *);
typedef int (*uk_store_get_u64_func_t)(void *, __u64 *);
typedef int (*uk_store_get_uptr_func_t)(void *, __uptr *);
typedef int (*uk_store_get_charp_func_t)(void *, char **);

/* Setter definitions */
typedef int (*uk_store_set_s8_func_t)(void *, __s8);
typedef int (*uk_store_set_u8_func_t)(void *, __u8);
typedef int (*uk_store_set_s16_func_t)(void *, __s16);
typedef int (*uk_store_set_u16_func_t)(void *, __u16);
typedef int (*uk_store_set_s32_func_t)(void *, __s32);
typedef int (*uk_store_set_u32_func_t)(void *, __u32);
typedef int (*uk_store_set_s64_func_t)(void *, __s64);
typedef int (*uk_store_set_u64_func_t)(void *, __u64);
typedef int (*uk_store_set_uptr_func_t)(void *, __uptr);
typedef int (*uk_store_set_charp_func_t)(void *, char *);

/* Cleanup definition */
typedef void (*uk_store_entry_freed_func_t)(void *);

struct uk_store_entry {
	/* Function getter pointer */
	union {
		uk_store_get_s8_func_t   s8;
		uk_store_get_u8_func_t   u8;
		uk_store_get_s16_func_t  s16;
		uk_store_get_u16_func_t  u16;
		uk_store_get_s32_func_t  s32;
		uk_store_get_u32_func_t  u32;
		uk_store_get_s64_func_t  s64;
		uk_store_get_u64_func_t  u64;
		uk_store_get_uptr_func_t uptr;
		uk_store_get_charp_func_t charp;
	} get;

	/* Function setter pointer */
	union {
		uk_store_set_s8_func_t   s8;
		uk_store_set_u8_func_t   u8;
		uk_store_set_s16_func_t  s16;
		uk_store_set_u16_func_t  u16;
		uk_store_set_s32_func_t  s32;
		uk_store_set_u32_func_t  u32;
		uk_store_set_s64_func_t  s64;
		uk_store_set_u64_func_t  u64;
		uk_store_set_uptr_func_t uptr;
		uk_store_set_charp_func_t charp;
	} set;

	/* The entry name */
	char *name;

	/* Entry flags */
	int flags;

	/* Entry getter/setter type */
	enum uk_store_entry_type type;

	/* A cookie for extra storage */
	void *cookie;
} __align8;

/* Used internally for storage */
struct uk_store_folder;
struct uk_store_folder_entry;

/* Flags if an entry is static or dynamic */
#define UK_STORE_ENTRY_FLAG_STATIC	(1 << 0)

/**
 * Checks if an entry is static
 *
 * @param entry the entry to check
 * @return the condition result
 */
#define UK_STORE_ENTRY_ISSTATIC(entry)	\
	((entry)->flags & UK_STORE_ENTRY_FLAG_STATIC)

/* Do not call directly */
#define __UK_STORE_STATIC_ENTRY(entry, lib_str, e_type, e_get, e_set)	\
	static const struct uk_store_entry				\
	__used __section(".uk_store_lib_" lib_str) __align8		\
	__uk_store_entries_list ## _ ## entry = {			\
		.name = STRINGIFY(entry),				\
		.type       = (uk_store_get_entry_type(e_type)),	\
		.get.e_type = (e_get),					\
		.set.e_type = (e_set),					\
		.flags      = UK_STORE_ENTRY_FLAG_STATIC,		\
		.cookie     = NULL					\
	}

/* Do not call directly */
#define _UK_STORE_STATIC_ENTRY(entry, lib_str, e_type, e_get, e_set)	\
	__UK_STORE_STATIC_ENTRY(entry, lib_str, e_type, e_get, e_set)

/**
 * Adds an entry to the entry section of a library.
 *
 * @param entry the entry in the section
 * @param e_type the type of the function
 * @param e_get getter pointer
 * @param e_set setter pointer
 */
#define UK_STORE_STATIC_ENTRY(entry, e_type, e_get, e_set)	\
	_UK_STORE_STATIC_ENTRY(entry, STRINGIFY(__LIBNAME__),	\
		e_type, e_get, e_set)

const struct uk_store_entry *
		_uk_store_dynamic_entry_create_u8(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_u8_func_t get, uk_store_set_u8_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_s8(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_s8_func_t get, uk_store_set_s8_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_u16(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_u16_func_t get, uk_store_set_u16_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_s16(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_s16_func_t get, uk_store_set_s16_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_u32(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_u32_func_t get, uk_store_set_u32_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_s32(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_s32_func_t get, uk_store_set_s32_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_u64(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_u64_func_t get, uk_store_set_u64_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_s64(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_s64_func_t get, uk_store_set_s64_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_uptr(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_uptr_func_t get, uk_store_set_uptr_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);
const struct uk_store_entry *
		_uk_store_dynamic_entry_create_charp(
		struct uk_store_folder *folder, const char *name,
		uk_store_get_charp_func_t get, uk_store_set_charp_func_t set,
		void *cookie, uk_store_entry_freed_func_t clean);

/**
 * Creates a new dynamic entry
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param fld the folder where to add the entry
 * @param n the name of the new entry (it is duplicated with strdup)
 * @param type the type of the new entry
 * @param g the getter function to save
 * @param s the setter function to save
 * @param c cookie for extra storage (optional)
 * @param f cleanup function (optional)
 * @return pointer to the created entry
 */
#define uk_store_dynamic_entry_create(fld, n, type, g, s, c, f)	\
	_uk_store_dynamic_entry_create_ ## type((fld), (n), (g), (s), (c), (f))

/**
 * Creates a new dynamic folder
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param name the name of the new folder
 * @return pointer to the alloc folder
 */
#define uk_store_dynamic_folder_alloc(name)	\
	_uk_store_dynamic_folder_alloc(uk_alloc_get_default(), (name))

struct uk_store_folder *
_uk_store_dynamic_folder_alloc(struct uk_alloc *a, const char *name);

/**
 * Adds a dynamic folder to a library
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param folder the folder to add it to
 * @return 0 on success and < 0 on failure
 */
#define uk_store_add_folder(folder)	\
	_uk_store_add_folder(uk_store_libid_self(), folder)

int
_uk_store_add_folder(unsigned int library_id, struct uk_store_folder *folder);

/**
 * Frees the memory of a dynamic folder and
 * the memory of the entries and deletes them
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param folder the folder to free
 */
#define uk_store_free_folder(folder)	\
	_uk_store_free_folder((folder))

void
_uk_store_free_folder(struct uk_store_folder **folder);

/**
 * Removes a dynamic folder from the folder list of the library
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param folder the folder to remove
 */
#define uk_store_remove_folder(folder)	\
	_uk_store_remove_folder((folder))

void
_uk_store_remove_folder(struct uk_store_folder *folder);

/**
 * Get a dynamic folder from a library
 *
 * @param folder_name the name of the folder to search for
 * @param library_id the id returned by uk_store_libid
 * @return the folder searched for
 */
#define uk_store_get_folder(library_id, folder_name)	\
	_uk_store_get_folder(library_id, folder_name)

struct uk_store_folder *
_uk_store_get_folder(unsigned int library_id, const char *name);

const struct uk_store_entry *
_uk_store_get_dynamic_entry(unsigned int libid, const char *f_name,
				const char *e_name);

const struct uk_store_entry *
_uk_store_get_static_entry(unsigned int libid, const char *e_name);

static inline const struct uk_store_entry *
_uk_store_get_entry(unsigned int libid, const char *f_name, const char *e_name)
{
	if (!f_name)
		return _uk_store_get_static_entry(libid, e_name);
	return _uk_store_get_dynamic_entry(libid, f_name, e_name);
}

/**
 * Searches for an entry in a folder in a library. Increases the refcount.
 *
 * @param libname the name of the library to search in
 * @param foldername the name of the folder to search in
 * @param entryname the name of the entry to search for
 * @return the found entry or NULL
 */
#define uk_store_get_entry(libname, foldername, entryname)	\
	_uk_store_get_entry(uk_store_libid(libname), foldername, entryname)


/**
 * Frees the memory of an entry
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param entry the entry to free
 */
#define uk_store_free_entry(entry)	\
	_uk_store_free_entry((entry))

void
_uk_store_free_entry(const struct uk_store_entry *entry);

/**
 * Decreases the refcount. When it reaches 0, the memory is freed
 *
 * Guard this with `#if CONFIG_LIBUKSTORE`
 *
 * @param entry the entry to release
 */
#define uk_store_release_entry(entry)	\
	_uk_store_release_entry((entry))

void
_uk_store_release_entry(const struct uk_store_entry **entry);

int
_uk_store_get_u8(const struct uk_store_entry *e, __u8 *out);
int
_uk_store_get_s8(const struct uk_store_entry *e, __s8 *out);
int
_uk_store_get_u16(const struct uk_store_entry *e, __u16 *out);
int
_uk_store_get_s16(const struct uk_store_entry *e, __s16 *out);
int
_uk_store_get_u32(const struct uk_store_entry *e, __u32 *out);
int
_uk_store_get_s32(const struct uk_store_entry *e, __s32 *out);
int
_uk_store_get_u64(const struct uk_store_entry *e, __u64 *out);
int
_uk_store_get_s64(const struct uk_store_entry *e, __s64 *out);
int
_uk_store_get_uptr(const struct uk_store_entry *e, __uptr *out);
int
_uk_store_get_charp(const struct uk_store_entry *e, char **out);

/**
 * Calls the getter to get it's value
 *
 * @param entry the entry to call the getter from
 * @param outtype the type of the variable used
 * @param out space to store the result of the call
 * @return 0 or error on fail
 */
#define uk_store_get_value(entry, outtype, out)	\
	_uk_store_get_ ## outtype((entry), (out))

int
_uk_store_set_u8(const struct uk_store_entry *e, __u8 val);
int
_uk_store_set_s8(const struct uk_store_entry *e, __s8 val);
int
_uk_store_set_u16(const struct uk_store_entry *e, __u16 val);
int
_uk_store_set_s16(const struct uk_store_entry *e, __s16 val);
int
_uk_store_set_u32(const struct uk_store_entry *e, __u32 val);
int
_uk_store_set_s32(const struct uk_store_entry *e, __s32 val);
int
_uk_store_set_u64(const struct uk_store_entry *e, __u64 val);
int
_uk_store_set_s64(const struct uk_store_entry *e, __s64 val);
int
_uk_store_set_uptr(const struct uk_store_entry *e, __uptr val);
int
_uk_store_set_charp(const struct uk_store_entry *e, const char *val);

/**
 * Calls the set to set a new value value
 *
 * @param entry the entry to call the setter from
 * @param intype the type of the variable used
 * @param in value to set
 * @return 0 or error on fail
 */
#define uk_store_set_value(entry, intype, in)	\
	_uk_store_set_ ## intype((entry), (in))

#ifdef __cplusplus
}
#endif

#endif /* __UK_STORE_H__ */
