#include <string>
#include <stdint.h>
#include <boost/python.hpp>

#include "security/OSRSM234.h"
#include "security/OpenSSL.h"
#include "Version.h"

using namespace std;
using namespace boost::python;

BOOST_PYTHON_MODULE(xSecurity)
{
	namespace python = boost::python;

	WRAPPER_F_LIST

	enum_<_enMSeqMode>("enMSeqMode")
			.value("MSeq_C1C2C3",MSeq_C1C2C3)
			.value("MSeq_C1C3C2",MSeq_C1C3C2);

	enum_<_enSM4Mode>("enSM4Mode")
			.value("SM4_M_ECB",SM4_M_ECB)
			.value("SM4_M_CBC",SM4_M_CBC)
			.value("SM4_M_CFB_128B",SM4_M_CFB_128B)
			.value("SM4_M_OFB_128B",SM4_M_OFB_128B)
			.value("SM4_M_CTR",SM4_M_CTR)
			.value("SM4_M_XTS_STEP",SM4_M_XTS_STEP)
			.value("SM4_M_XTS_ONCE",SM4_M_XTS_ONCE);

	class_<OpenSSLEVPCipher>("OpenSSL",init<>())
			.def("ChgAlgorithm",&OpenSSLEVPCipher::ChgAlgorithm)
			.def("SetKey",&OpenSSLEVPCipher::SetKey)
			.def("GetKey",&OpenSSLEVPCipher::GetKey)
			.def("SetIV",&OpenSSLEVPCipher::SetIV)
			.def("GetIV",&OpenSSLEVPCipher::GetIV)
			.def("Encrypt",&OpenSSLEVPCipher::Encrypt)
			.def("Decrypt",&OpenSSLEVPCipher::Decrypt);

	class_<OSRSM2>("OSRSM2",init<const char*>())
			.add_property("ErrorCode",&OSRSM2::GetLastError)
			.add_property("MSeqMode",&OSRSM2::GetMSeqMode,&OSRSM2::SetMSeqMode)
			.def("Dump",&OSRSM2::Dump)
			.def("SetPKey",&OSRSM2::SetPKey)
			.def("SetEKey",&OSRSM2::SetEKey)
			.def("SetZKey",&OSRSM2::SetZKey)
			.def("SetSign",&OSRSM2::SetSign)
			.def("SetUsrID",&OSRSM2::SetUsrID)
			.def("GetPKey",&OSRSM2::GetPKey)
			.def("GetEKey",&OSRSM2::GetEKey)
			.def("GetZKey",&OSRSM2::GetZKey)
			.def("GetSign",&OSRSM2::GetSign)
			.def("GetUsrID",&OSRSM2::GetUsrID)
			.def("Sign",&OSRSM2::Sign)
			.def("Verify",&OSRSM2::Verify)
			.def("Encrypt",&OSRSM2::Encrypt)
			.def("Decrypt",&OSRSM2::Decrypt);

	class_<OSRSM3>("OSRSM3")
			.add_property("ErrorCode",&OSRSM3::GetLastError)
			.def("Dump",&OSRSM3::Dump)
			.def("GetDigest",&OSRSM3::GetDigest)
			.def("Step1_Init",&OSRSM3::Step1_Init)
			.def("Step2_Process",&OSRSM3::Step2_Process)
			.def("Step3_Done",&OSRSM3::Step3_Done)
			.def("GetDigestHash",&OSRSM3::GetDigestHash);

	class_<OSRSM4>("OSRSM4")
			.add_property("ErrorCode",&OSRSM4::GetLastError)
			.add_property("SM4Mode",&OSRSM4::GetSM4Mode,&OSRSM4::SetSM4Mode)
			.def("Dump",&OSRSM4::Dump)
			.def("SetSM4Key",&OSRSM4::SetSM4Key)
			.def("GetSM4Key",&OSRSM4::GetSM4Key)
			.def("SetSM4IV",&OSRSM4::SetSM4IV)
			.def("GetSM4IV",&OSRSM4::GetSM4IV)
			.def("SetSM4CTR",&OSRSM4::SetSM4CTR)
			.def("GetSM4CTR",&OSRSM4::GetSM4CTR)
			.def("Encrypt",&OSRSM4::Encrypt)
			.def("Decrypt",&OSRSM4::Decrypt);
}

