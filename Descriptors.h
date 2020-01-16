#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

    #include <LUFA/Drivers/USB/USB.h>
    #include <avr/pgmspace.h>

    #define VENDOR_IN_EPADDR               (ENDPOINT_DIR_IN  | 3)
    #define VENDOR_OUT_EPADDR              (ENDPOINT_DIR_OUT | 4)
    #define VENDOR_IO_EPSIZE               64

    typedef struct
    {
        USB_Descriptor_Configuration_Header_t Config;
        USB_Descriptor_Interface_t            Vendor_Interface;
        USB_Descriptor_Endpoint_t             Vendor_DataInEndpoint;
        USB_Descriptor_Endpoint_t             Vendor_DataOutEndpoint;
        USB_Descriptor_DeviceQualifier_t	  Device_Qualifier;
    } USB_Descriptor_Configuration_t;

    enum InterfaceDescriptors_t
    {
        INTERFACE_ID_Vendor = 0,
    };

    enum StringDescriptors_t
    {
        STRING_ID_Language     = 0,
        STRING_ID_Manufacturer = 1,
        STRING_ID_Product      = 2,
    };

    uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                        const uint16_t wIndex,
                                        const void** const DescriptorAddress)
                                        ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);
#endif