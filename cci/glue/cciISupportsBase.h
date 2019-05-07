#ifndef cciISupportsBase_h
#define cciISupportsBase_h

#ifndef cciCore_h
#include "cciCore.h"
#endif

#ifndef cciId_h
#include "cciId.h"
#endif

#define CCI_ISUPPORTS_IID                              \
  { 0x00000000, 0x0000, 0x0000,                         \
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

/**
 * Basic component object model interface. Objects which implement
 * this interface support runtime interface discovery (QueryInterface)
 * and a reference counted memory model (AddRef/Release). This is
 * modelled after the win32 IUnknown API.
 */
class CCI_NO_VTABLE cciISupports {
public:

  CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_ISUPPORTS_IID)

  //@{
  /**
   * A run time mechanism for interface discovery.
   * @param aIID [in] A requested interface IID
   * @param aInstancePtr [out] A pointer to an interface pointer to
   * receive the result.
   * @return <bCCI_OK</b> if the interface is supported by the associated
   * instance, <b>CCI_NOINTERFACE</b> if it is not.
   *
   * aInstancePtr must not be null.
   */
  CCI_IMETHOD QueryInterface(DM_REFIID aIID, void** aInstancePtr) = 0;
  /**
   * Increases the reference count for this interface.
   * The associated instance will not be deleted unless
   * the reference count is returned to zero.
   *
   * @return The resulting reference count.
   */
  CCI_IMETHOD_(cci_refcnt) AddRef(void) = 0;

  /**
   * Decreases the reference count for this interface.
   * Generally, if the reference count returns to zero,
   * the associated instance is deleted.
   *
   * @return The resulting reference count.
   */
  CCI_IMETHOD_(cci_refcnt) Release(void) = 0;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciISupports, CCI_ISUPPORTS_IID)


#endif // cciSupports_h

