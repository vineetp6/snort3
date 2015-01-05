/*
** Copyright (C) 2014-2015 Cisco and/or its affiliates. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
// cd_trans_bridge.cc author Josh Rosenbaum <jrosenba@cisco.com>




#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "framework/codec.h"
#include "codecs/codec_module.h"
#include "codecs/codec_events.h"

#include "protocols/eth.h"
#include "protocols/protocol_ids.h"
#include "protocols/packet.h"

namespace
{

#define CD_TRANSBRIDGE_NAME "trans_bridge"
#define CD_TRANSBRIDGE_HELP "support for trans-bridging"

class TransbridgeCodec : public Codec
{
public:
    TransbridgeCodec() : Codec(CD_TRANSBRIDGE_NAME){};
    ~TransbridgeCodec(){};


    void get_protocol_ids(std::vector<uint16_t>& v) override;
    bool decode(const RawData&, CodecData&, DecodeData&) override;
};

} // anonymous namespace


void TransbridgeCodec::get_protocol_ids(std::vector<uint16_t>& v)
{ v.push_back(ETHERTYPE_TRANS_ETHER_BRIDGING); }


/*
 * Function: DecodeTransBridging(uint8_t *, const uint32_t, Packet)
 *
 * Purpose: Decode Transparent Ethernet Bridging
 *
 * Arguments: pkt => pointer to the real live packet data
 *            len => length of remaining data in packet
 *            p => pointer to the decoded packet struct
 *
 *
 * Returns: void function
 *
 * Note: This is basically the code from DecodeEthPkt but the calling
 * convention needed to be changed and the stuff at the beginning
 * wasn't needed since we are already deep into the packet
 */
bool TransbridgeCodec::decode(const RawData& raw, CodecData& codec, DecodeData&)
{
    if(raw.len < eth::ETH_HEADER_LEN)
    {
        codec_events::decoder_event(codec, DECODE_GRE_TRANS_DGRAM_LT_TRANSHDR);
        return false;
    }

    /* The Packet struct's ethernet header will now point to the inner ethernet
     * header of the packet
     */
    const eth::EtherHdr* const eh =
        reinterpret_cast<const eth::EtherHdr*>(raw.data);

    codec.proto_bits |= PROTO_BIT__ETH;
    codec.lyr_len = eth::ETH_HEADER_LEN;
    codec.next_prot_id = ntohs(eh->ether_type);
    return true;
}


//-------------------------------------------------------------------------
// api
//-------------------------------------------------------------------------

static Codec* ctor(Module*)
{ return new TransbridgeCodec(); }

static void dtor(Codec *cd)
{ delete cd; }

static const CodecApi transbridge_api =
{
    {
        PT_CODEC,
        CD_TRANSBRIDGE_NAME,
        CD_TRANSBRIDGE_HELP,
        CDAPI_PLUGIN_V0,
        0,
        nullptr,
        nullptr
    },
    nullptr, // pinit
    nullptr, // pterm
    nullptr, // tinit
    nullptr, // tterm
    ctor, // ctor
    dtor, // dtor
};


#ifdef BUILDING_SO
SO_PUBLIC const BaseApi* snort_plugins[] =
{
    &transbridge_api.base,
    nullptr
};
#else
const BaseApi* cd_transbridge = &transbridge_api.base;
#endif




