#ifndef cciErrors_h
#define cciErrors_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

#define CCI_ERROR_MODULE_COM   1
#define CCI_ERROR_MODULE_BASE  2
#define CCI_ERROR_MODULE_FILES 13

#define CCI_ERROR_MODULE_BASE_OFFSET 0x45

/* CCI Error Messages code */
#define CCI_ERROR_SEVERITY_SUCCESS       0
#define CCI_ERROR_SEVERITY_WARNING       CCI_ERROR_SEVERITY_SUCCESS
#define CCI_ERROR_SEVERITY_ERROR         1

#define CCI_FAILED(_result)    (DM_UNLIKELY((_result) & 0x80000000))
#define CCI_SUCCEEDED(_result) (DM_LIKELY(!((_result) & 0x80000000)))

/**
 * @name Macros for retrieving error bits
 */

#define CCI_GET_ERROR_CODE( err )     ((err) & 0xffff)
#define CCI_GET_ERROR_MODULE( err )   (((((err) >> 16) - CCI_ERROR_MODULE_BASE_OFFSET) & 0x1fff))
#define CCI_GET_ERROR_SEVERITY( err ) (((err) >> 31) & 0x1)

#define CCI_ERROR_GENERATE(sev,module,code) \
    ((cci_result) (((dm_uint32)(sev)<<31) | ((dm_uint32)(module+CCI_ERROR_MODULE_BASE_OFFSET)<<16) | ((dm_uint32)(code))) )

#define CCI_ERROR_GENERATE_SUCCESS(module,code) \
    ((cci_result) (((dm_uint32)(CCI_ERROR_SEVERITY_SUCCESS)<<31) | ((dm_uint32)(module+CCI_ERROR_MODULE_BASE_OFFSET)<<16) | ((dm_uint32)(code))) )

#define CCI_ERROR_GENERATE_FAILURE(module,code) \
    ((cci_result) (((dm_uint32)(CCI_ERROR_SEVERITY_ERROR)<<31) | ((dm_uint32)(module+CCI_ERROR_MODULE_BASE_OFFSET)<<16) | ((dm_uint32)(code))) )


#define CCI_OK 0

/* Standard "it worked" return value */
#define CCI_OK                              0

#define CCI_ERROR_BASE                      ((cci_result) 0xC1F30000)

/* Returned when an instance is not initialized */
#define CCI_ERROR_NOT_INITIALIZED           (CCI_ERROR_BASE + 1)

/* Returned when an instance is already initialized */
#define CCI_ERROR_ALREADY_INITIALIZED       (CCI_ERROR_BASE + 2)

/* Returned by a not implemented function */
#define CCI_ERROR_NOT_IMPLEMENTED           ((cci_result) 0x80004001L)

/* Returned when a given interface is not supported. */
#define CCI_NOINTERFACE                     ((cci_result) 0x80004002L)
#define CCI_ERROR_NO_INTERFACE              CCI_NOINTERFACE

#define CCI_ERROR_INVALID_POINTER           ((cci_result) 0x80004003L)
#define CCI_ERROR_NULL_POINTER              CCI_ERROR_INVALID_POINTER

/* Returned when a function aborts */
#define CCI_ERROR_ABORT                     ((cci_result) 0x80004004L)

/* Returned when a function fails */
#define CCI_ERROR_FAILURE                   ((cci_result) 0x80004005L)

/* Returned when an unexpected error occurs */
#define CCI_ERROR_UNEXPECTED                ((cci_result) 0x8000ffffL)

/* Returned when a memory allocation fails */
#define CCI_ERROR_OUT_OF_MEMORY             ((cci_result) 0x8007000eL)

/* Returned when an illegal value is passed */
#define CCI_ERROR_ILLEGAL_VALUE             ((cci_result) 0x80070057L)
#define CCI_ERROR_INVALID_ARG               CCI_ERROR_ILLEGAL_VALUE

/* Returned when a class doesn't allow aggregation */
#define CCI_ERROR_NO_AGGREGATION            ((cci_result) 0x80040110L)

/* Returned when an operation can't complete due to an unavailable resource */
#define CCI_ERROR_NOT_AVAILABLE             ((cci_result) 0x80040111L)

/* Returned when a class is not registered */
#define CCI_ERROR_FACTORY_NOT_REGISTERED    ((cci_result) 0x80040154L)

/* Returned when a class cannot be registered, but may be tried again later */
#define CCI_ERROR_FACTORY_REGISTER_AGAIN    ((cci_result) 0x80040155L)

/* Returned when a dynamically loaded factory couldn't be found */
#define CCI_ERROR_FACTORY_NOT_LOADED        ((cci_result) 0x800401f8L)

/* Returned when a factory doesn't support signatures */
#define CCI_ERROR_FACTORY_NO_SIGNATURE_SUPPORT \
                                           (CCI_ERROR_BASE + 0x101)

/* Returned when a factory already is registered */
#define CCI_ERROR_FACTORY_EXISTS            (CCI_ERROR_BASE + 0x100)

// File module error
#define CCI_ERROR_FILE_UNRECOGNIZED_PATH         CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 1)
#define CCI_ERROR_FILE_UNRESOLVABLE_SYMLINK      CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 2)
#define CCI_ERROR_FILE_EXECUTION_FAILED          CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 3)
#define CCI_ERROR_FILE_UNKNOWN_TYPE              CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 4)
#define CCI_ERROR_FILE_DESTINATION_NOT_DIR       CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 5)
#define CCI_ERROR_FILE_TARGET_DOES_NOT_EXIST     CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 6)
#define CCI_ERROR_FILE_COPY_OR_MOVE_FAILED       CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 7)
#define CCI_ERROR_FILE_ALREADY_EXISTS            CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 8)
#define CCI_ERROR_FILE_INVALID_PATH              CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 9)
#define CCI_ERROR_FILE_DISK_FULL                 CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 10)
#define CCI_ERROR_FILE_CORRUPTED                 CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 11)
#define CCI_ERROR_FILE_NOT_DIRECTORY             CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 12)
#define CCI_ERROR_FILE_IS_DIRECTORY              CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 13)
#define CCI_ERROR_FILE_IS_LOCKED                 CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 14)
#define CCI_ERROR_FILE_TOO_BIG                   CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 15)
#define CCI_ERROR_FILE_NO_DEVICE_SPACE           CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 16)
#define CCI_ERROR_FILE_NAME_TOO_LONG             CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 17)
#define CCI_ERROR_FILE_NOT_FOUND                 CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 18)
#define CCI_ERROR_FILE_READ_ONLY                 CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 19)
#define CCI_ERROR_FILE_DIR_NOT_EMPTY             CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 20)
#define CCI_ERROR_FILE_ACCESS_DENIED             CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_FILES, 21)

/////////////////////////////////////////////////////////////////////////

#define CCI_ERROR_SERVICE_NOT_AVAILABLE  CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_COM, 22)
#define CCI_ERROR_REVOKED_OBJECT         CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_COM, 23)
#define CCI_ERROR_OUT_OF_RANGE           CCI_ERROR_GENERATE_FAILURE(CCI_ERROR_MODULE_COM, 24)

/////////////////////////////////////////////////////////////////////////
#endif // cciErrors_h


