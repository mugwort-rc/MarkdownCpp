/*
 * htmlentitydefs.hpp
 *
 *  Created on: 2013/11/13
 *      Author: mugwort_rc
 */

#ifndef HTMLENTITYDEFS_HPP_
#define HTMLENTITYDEFS_HPP_

namespace markdown{

//! from htmlentitydefs.py
std::map<wchar_t, std::wstring> codepoint2name = {
    std::make_pair(L'\u00c6', L"AElig"),    //!< latin capital letter AE = latin capital ligature AE, U+00C6 ISOlat1
    std::make_pair(L'\u00c1', L"Aacute"),   //!< latin capital letter A with acute, U+00C1 ISOlat1
    std::make_pair(L'\u00c2', L"Acirc"),    //!< latin capital letter A with circumflex, U+00C2 ISOlat1
    std::make_pair(L'\u00c0', L"Agrave"),   //!< latin capital letter A with grave = latin capital letter A grave, U+00C0 ISOlat1
    std::make_pair(L'\u0391', L"Alpha"),    //!< greek capital letter alpha, U+0391
    std::make_pair(L'\u00c5', L"Aring"),    //!< latin capital letter A with ring above = latin capital letter A ring, U+00C5 ISOlat1
    std::make_pair(L'\u00c3', L"Atilde"),   //!< latin capital letter A with tilde, U+00C3 ISOlat1
    std::make_pair(L'\u00c4', L"Auml"),     //!< latin capital letter A with diaeresis, U+00C4 ISOlat1
    std::make_pair(L'\u0392', L"Beta"),     //!< greek capital letter beta, U+0392
    std::make_pair(L'\u00c7', L"Ccedil"),   //!< latin capital letter C with cedilla, U+00C7 ISOlat1
    std::make_pair(L'\u03a7', L"Chi"),      //!< greek capital letter chi, U+03A7
    std::make_pair(L'\u2021', L"Dagger"),   //!< double dagger, U+2021 ISOpub
    std::make_pair(L'\u0394', L"Delta"),    //!< greek capital letter delta, U+0394 ISOgrk3
    std::make_pair(L'\u00d0', L"ETH"),      //!< latin capital letter ETH, U+00D0 ISOlat1
    std::make_pair(L'\u00c9', L"Eacute"),   //!< latin capital letter E with acute, U+00C9 ISOlat1
    std::make_pair(L'\u00ca', L"Ecirc"),    //!< latin capital letter E with circumflex, U+00CA ISOlat1
    std::make_pair(L'\u00c8', L"Egrave"),   //!< latin capital letter E with grave, U+00C8 ISOlat1
    std::make_pair(L'\u0395', L"Epsilon"),  //!< greek capital letter epsilon, U+0395
    std::make_pair(L'\u0397', L"Eta"),      //!< greek capital letter eta, U+0397
    std::make_pair(L'\u00cb', L"Euml"),     //!< latin capital letter E with diaeresis, U+00CB ISOlat1
    std::make_pair(L'\u0393', L"Gamma"),    //!< greek capital letter gamma, U+0393 ISOgrk3
    std::make_pair(L'\u00cd', L"Iacute"),   //!< latin capital letter I with acute, U+00CD ISOlat1
    std::make_pair(L'\u00ce', L"Icirc"),    //!< latin capital letter I with circumflex, U+00CE ISOlat1
    std::make_pair(L'\u00cc', L"Igrave"),   //!< latin capital letter I with grave, U+00CC ISOlat1
    std::make_pair(L'\u0399', L"Iota"),     //!< greek capital letter iota, U+0399
    std::make_pair(L'\u00cf', L"Iuml"),     //!< latin capital letter I with diaeresis, U+00CF ISOlat1
    std::make_pair(L'\u039a', L"Kappa"),    //!< greek capital letter kappa, U+039A
    std::make_pair(L'\u039b', L"Lambda"),   //!< greek capital letter lambda, U+039B ISOgrk3
    std::make_pair(L'\u039c', L"Mu"),       //!< greek capital letter mu, U+039C
    std::make_pair(L'\u00d1', L"Ntilde"),   //!< latin capital letter N with tilde, U+00D1 ISOlat1
    std::make_pair(L'\u039d', L"Nu"),       //!< greek capital letter nu, U+039D
    std::make_pair(L'\u0152', L"OElig"),    //!< latin capital ligature OE, U+0152 ISOlat2
    std::make_pair(L'\u00d3', L"Oacute"),   //!< latin capital letter O with acute, U+00D3 ISOlat1
    std::make_pair(L'\u00d4', L"Ocirc"),    //!< latin capital letter O with circumflex, U+00D4 ISOlat1
    std::make_pair(L'\u00d2', L"Ograve"),   //!< latin capital letter O with grave, U+00D2 ISOlat1
    std::make_pair(L'\u03a9', L"Omega"),    //!< greek capital letter omega, U+03A9 ISOgrk3
    std::make_pair(L'\u039f', L"Omicron"),  //!< greek capital letter omicron, U+039F
    std::make_pair(L'\u00d8', L"Oslash"),   //!< latin capital letter O with stroke = latin capital letter O slash, U+00D8 ISOlat1
    std::make_pair(L'\u00d5', L"Otilde"),   //!< latin capital letter O with tilde, U+00D5 ISOlat1
    std::make_pair(L'\u00d6', L"Ouml"),     //!< latin capital letter O with diaeresis, U+00D6 ISOlat1
    std::make_pair(L'\u03a6', L"Phi"),      //!< greek capital letter phi, U+03A6 ISOgrk3
    std::make_pair(L'\u03a0', L"Pi"),       //!< greek capital letter pi, U+03A0 ISOgrk3
    std::make_pair(L'\u2033', L"Prime"),    //!< double prime = seconds = inches, U+2033 ISOtech
    std::make_pair(L'\u03a8', L"Psi"),      //!< greek capital letter psi, U+03A8 ISOgrk3
    std::make_pair(L'\u03a1', L"Rho"),      //!< greek capital letter rho, U+03A1
    std::make_pair(L'\u0160', L"Scaron"),   //!< latin capital letter S with caron, U+0160 ISOlat2
    std::make_pair(L'\u03a3', L"Sigma"),    //!< greek capital letter sigma, U+03A3 ISOgrk3
    std::make_pair(L'\u00de', L"THORN"),    //!< latin capital letter THORN, U+00DE ISOlat1
    std::make_pair(L'\u03a4', L"Tau"),      //!< greek capital letter tau, U+03A4
    std::make_pair(L'\u0398', L"Theta"),    //!< greek capital letter theta, U+0398 ISOgrk3
    std::make_pair(L'\u00da', L"Uacute"),   //!< latin capital letter U with acute, U+00DA ISOlat1
    std::make_pair(L'\u00db', L"Ucirc"),    //!< latin capital letter U with circumflex, U+00DB ISOlat1
    std::make_pair(L'\u00d9', L"Ugrave"),   //!< latin capital letter U with grave, U+00D9 ISOlat1
    std::make_pair(L'\u03a5', L"Upsilon"),  //!< greek capital letter upsilon, U+03A5 ISOgrk3
    std::make_pair(L'\u00dc', L"Uuml"),     //!< latin capital letter U with diaeresis, U+00DC ISOlat1
    std::make_pair(L'\u039e', L"Xi"),       //!< greek capital letter xi, U+039E ISOgrk3
    std::make_pair(L'\u00dd', L"Yacute"),   //!< latin capital letter Y with acute, U+00DD ISOlat1
    std::make_pair(L'\u0178', L"Yuml"),     //!< latin capital letter Y with diaeresis, U+0178 ISOlat2
    std::make_pair(L'\u0396', L"Zeta"),     //!< greek capital letter zeta, U+0396
    std::make_pair(L'\u00e1', L"aacute"),   //!< latin small letter a with acute, U+00E1 ISOlat1
    std::make_pair(L'\u00e2', L"acirc"),    //!< latin small letter a with circumflex, U+00E2 ISOlat1
    std::make_pair(L'\u00b4', L"acute"),    //!< acute accent = spacing acute, U+00B4 ISOdia
    std::make_pair(L'\u00e6', L"aelig"),    //!< latin small letter ae = latin small ligature ae, U+00E6 ISOlat1
    std::make_pair(L'\u00e0', L"agrave"),   //!< latin small letter a with grave = latin small letter a grave, U+00E0 ISOlat1
    std::make_pair(L'\u2135', L"alefsym"),  //!< alef symbol = first transfinite cardinal, U+2135 NEW
    std::make_pair(L'\u03b1', L"alpha"),    //!< greek small letter alpha, U+03B1 ISOgrk3
    std::make_pair(L'\u0026', L"amp"),      //!< ampersand, U+0026 ISOnum
    std::make_pair(L'\u2227', L"and"),      //!< logical and = wedge, U+2227 ISOtech
    std::make_pair(L'\u2220', L"ang"),      //!< angle, U+2220 ISOamso
    std::make_pair(L'\u00e5', L"aring"),    //!< latin small letter a with ring above = latin small letter a ring, U+00E5 ISOlat1
    std::make_pair(L'\u2248', L"asymp"),    //!< almost equal to = asymptotic to, U+2248 ISOamsr
    std::make_pair(L'\u00e3', L"atilde"),   //!< latin small letter a with tilde, U+00E3 ISOlat1
    std::make_pair(L'\u00e4', L"auml"),     //!< latin small letter a with diaeresis, U+00E4 ISOlat1
    std::make_pair(L'\u201e', L"bdquo"),    //!< double low-9 quotation mark, U+201E NEW
    std::make_pair(L'\u03b2', L"beta"),     //!< greek small letter beta, U+03B2 ISOgrk3
    std::make_pair(L'\u00a6', L"brvbar"),   //!< broken bar = broken vertical bar, U+00A6 ISOnum
    std::make_pair(L'\u2022', L"bull"),     //!< bullet = black small circle, U+2022 ISOpub
    std::make_pair(L'\u2229', L"cap"),      //!< intersection = cap, U+2229 ISOtech
    std::make_pair(L'\u00e7', L"ccedil"),   //!< latin small letter c with cedilla, U+00E7 ISOlat1
    std::make_pair(L'\u00b8', L"cedil"),    //!< cedilla = spacing cedilla, U+00B8 ISOdia
    std::make_pair(L'\u00a2', L"cent"),     //!< cent sign, U+00A2 ISOnum
    std::make_pair(L'\u03c7', L"chi"),      //!< greek small letter chi, U+03C7 ISOgrk3
    std::make_pair(L'\u02c6', L"circ"),     //!< modifier letter circumflex accent, U+02C6 ISOpub
    std::make_pair(L'\u2663', L"clubs"),    //!< black club suit = shamrock, U+2663 ISOpub
    std::make_pair(L'\u2245', L"cong"),     //!< approximately equal to, U+2245 ISOtech
    std::make_pair(L'\u00a9', L"copy"),     //!< copyright sign, U+00A9 ISOnum
    std::make_pair(L'\u21b5', L"crarr"),    //!< downwards arrow with corner leftwards = carriage return, U+21B5 NEW
    std::make_pair(L'\u222a', L"cup"),      //!< union = cup, U+222A ISOtech
    std::make_pair(L'\u00a4', L"curren"),   //!< currency sign, U+00A4 ISOnum
    std::make_pair(L'\u21d3', L"dArr"),     //!< downwards double arrow, U+21D3 ISOamsa
    std::make_pair(L'\u2020', L"dagger"),   //!< dagger, U+2020 ISOpub
    std::make_pair(L'\u2193', L"darr"),     //!< downwards arrow, U+2193 ISOnum
    std::make_pair(L'\u00b0', L"deg"),      //!< degree sign, U+00B0 ISOnum
    std::make_pair(L'\u03b4', L"delta"),    //!< greek small letter delta, U+03B4 ISOgrk3
    std::make_pair(L'\u2666', L"diams"),    //!< black diamond suit, U+2666 ISOpub
    std::make_pair(L'\u00f7', L"divide"),   //!< division sign, U+00F7 ISOnum
    std::make_pair(L'\u00e9', L"eacute"),   //!< latin small letter e with acute, U+00E9 ISOlat1
    std::make_pair(L'\u00ea', L"ecirc"),    //!< latin small letter e with circumflex, U+00EA ISOlat1
    std::make_pair(L'\u00e8', L"egrave"),   //!< latin small letter e with grave, U+00E8 ISOlat1
    std::make_pair(L'\u2205', L"empty"),    //!< empty set = null set = diameter, U+2205 ISOamso
    std::make_pair(L'\u2003', L"emsp"),     //!< em space, U+2003 ISOpub
    std::make_pair(L'\u2002', L"ensp"),     //!< en space, U+2002 ISOpub
    std::make_pair(L'\u03b5', L"epsilon"),  //!< greek small letter epsilon, U+03B5 ISOgrk3
    std::make_pair(L'\u2261', L"equiv"),    //!< identical to, U+2261 ISOtech
    std::make_pair(L'\u03b7', L"eta"),      //!< greek small letter eta, U+03B7 ISOgrk3
    std::make_pair(L'\u00f0', L"eth"),      //!< latin small letter eth, U+00F0 ISOlat1
    std::make_pair(L'\u00eb', L"euml"),     //!< latin small letter e with diaeresis, U+00EB ISOlat1
    std::make_pair(L'\u20ac', L"euro"),     //!< euro sign, U+20AC NEW
    std::make_pair(L'\u2203', L"exist"),    //!< there exists, U+2203 ISOtech
    std::make_pair(L'\u0192', L"fnof"),     //!< latin small f with hook = function = florin, U+0192 ISOtech
    std::make_pair(L'\u2200', L"forall"),   //!< for all, U+2200 ISOtech
    std::make_pair(L'\u00bd', L"frac12"),   //!< vulgar fraction one half = fraction one half, U+00BD ISOnum
    std::make_pair(L'\u00bc', L"frac14"),   //!< vulgar fraction one quarter = fraction one quarter, U+00BC ISOnum
    std::make_pair(L'\u00be', L"frac34"),   //!< vulgar fraction three quarters = fraction three quarters, U+00BE ISOnum
    std::make_pair(L'\u2044', L"frasl"),    //!< fraction slash, U+2044 NEW
    std::make_pair(L'\u03b3', L"gamma"),    //!< greek small letter gamma, U+03B3 ISOgrk3
    std::make_pair(L'\u2265', L"ge"),       //!< greater-than or equal to, U+2265 ISOtech
    std::make_pair(L'\u003e', L"gt"),       //!< greater-than sign, U+003E ISOnum
    std::make_pair(L'\u21d4', L"hArr"),     //!< left right double arrow, U+21D4 ISOamsa
    std::make_pair(L'\u2194', L"harr"),     //!< left right arrow, U+2194 ISOamsa
    std::make_pair(L'\u2665', L"hearts"),   //!< black heart suit = valentine, U+2665 ISOpub
    std::make_pair(L'\u2026', L"hellip"),   //!< horizontal ellipsis = three dot leader, U+2026 ISOpub
    std::make_pair(L'\u00ed', L"iacute"),   //!< latin small letter i with acute, U+00ED ISOlat1
    std::make_pair(L'\u00ee', L"icirc"),    //!< latin small letter i with circumflex, U+00EE ISOlat1
    std::make_pair(L'\u00a1', L"iexcl"),    //!< inverted exclamation mark, U+00A1 ISOnum
    std::make_pair(L'\u00ec', L"igrave"),   //!< latin small letter i with grave, U+00EC ISOlat1
    std::make_pair(L'\u2111', L"image"),    //!< blackletter capital I = imaginary part, U+2111 ISOamso
    std::make_pair(L'\u221e', L"infin"),    //!< infinity, U+221E ISOtech
    std::make_pair(L'\u222b', L"int"),      //!< integral, U+222B ISOtech
    std::make_pair(L'\u03b9', L"iota"),     //!< greek small letter iota, U+03B9 ISOgrk3
    std::make_pair(L'\u00bf', L"iquest"),   //!< inverted question mark = turned question mark, U+00BF ISOnum
    std::make_pair(L'\u2208', L"isin"),     //!< element of, U+2208 ISOtech
    std::make_pair(L'\u00ef', L"iuml"),     //!< latin small letter i with diaeresis, U+00EF ISOlat1
    std::make_pair(L'\u03ba', L"kappa"),    //!< greek small letter kappa, U+03BA ISOgrk3
    std::make_pair(L'\u21d0', L"lArr"),     //!< leftwards double arrow, U+21D0 ISOtech
    std::make_pair(L'\u03bb', L"lambda"),   //!< greek small letter lambda, U+03BB ISOgrk3
    std::make_pair(L'\u2329', L"lang"),     //!< left-pointing angle bracket = bra, U+2329 ISOtech
    std::make_pair(L'\u00ab', L"laquo"),    //!< left-pointing double angle quotation mark = left pointing guillemet, U+00AB ISOnum
    std::make_pair(L'\u2190', L"larr"),     //!< leftwards arrow, U+2190 ISOnum
    std::make_pair(L'\u2308', L"lceil"),    //!< left ceiling = apl upstile, U+2308 ISOamsc
    std::make_pair(L'\u201c', L"ldquo"),    //!< left double quotation mark, U+201C ISOnum
    std::make_pair(L'\u2264', L"le"),       //!< less-than or equal to, U+2264 ISOtech
    std::make_pair(L'\u230a', L"lfloor"),   //!< left floor = apl downstile, U+230A ISOamsc
    std::make_pair(L'\u2217', L"lowast"),   //!< asterisk operator, U+2217 ISOtech
    std::make_pair(L'\u25ca', L"loz"),      //!< lozenge, U+25CA ISOpub
    std::make_pair(L'\u200e', L"lrm"),      //!< left-to-right mark, U+200E NEW RFC 2070
    std::make_pair(L'\u2039', L"lsaquo"),   //!< single left-pointing angle quotation mark, U+2039 ISO proposed
    std::make_pair(L'\u2018', L"lsquo"),    //!< left single quotation mark, U+2018 ISOnum
    std::make_pair(L'\u003c', L"lt"),       //!< less-than sign, U+003C ISOnum
    std::make_pair(L'\u00af', L"macr"),     //!< macron = spacing macron = overline = APL overbar, U+00AF ISOdia
    std::make_pair(L'\u2014', L"mdash"),    //!< em dash, U+2014 ISOpub
    std::make_pair(L'\u00b5', L"micro"),    //!< micro sign, U+00B5 ISOnum
    std::make_pair(L'\u00b7', L"middot"),   //!< middle dot = Georgian comma = Greek middle dot, U+00B7 ISOnum
    std::make_pair(L'\u2212', L"minus"),    //!< minus sign, U+2212 ISOtech
    std::make_pair(L'\u03bc', L"mu"),       //!< greek small letter mu, U+03BC ISOgrk3
    std::make_pair(L'\u2207', L"nabla"),    //!< nabla = backward difference, U+2207 ISOtech
    std::make_pair(L'\u00a0', L"nbsp"),     //!< no-break space = non-breaking space, U+00A0 ISOnum
    std::make_pair(L'\u2013', L"ndash"),    //!< en dash, U+2013 ISOpub
    std::make_pair(L'\u2260', L"ne"),       //!< not equal to, U+2260 ISOtech
    std::make_pair(L'\u220b', L"ni"),       //!< contains as member, U+220B ISOtech
    std::make_pair(L'\u00ac', L"not"),      //!< not sign, U+00AC ISOnum
    std::make_pair(L'\u2209', L"notin"),    //!< not an element of, U+2209 ISOtech
    std::make_pair(L'\u2284', L"nsub"),     //!< not a subset of, U+2284 ISOamsn
    std::make_pair(L'\u00f1', L"ntilde"),   //!< latin small letter n with tilde, U+00F1 ISOlat1
    std::make_pair(L'\u03bd', L"nu"),       //!< greek small letter nu, U+03BD ISOgrk3
    std::make_pair(L'\u00f3', L"oacute"),   //!< latin small letter o with acute, U+00F3 ISOlat1
    std::make_pair(L'\u00f4', L"ocirc"),    //!< latin small letter o with circumflex, U+00F4 ISOlat1
    std::make_pair(L'\u0153', L"oelig"),    //!< latin small ligature oe, U+0153 ISOlat2
    std::make_pair(L'\u00f2', L"ograve"),   //!< latin small letter o with grave, U+00F2 ISOlat1
    std::make_pair(L'\u203e', L"oline"),    //!< overline = spacing overscore, U+203E NEW
    std::make_pair(L'\u03c9', L"omega"),    //!< greek small letter omega, U+03C9 ISOgrk3
    std::make_pair(L'\u03bf', L"omicron"),  //!< greek small letter omicron, U+03BF NEW
    std::make_pair(L'\u2295', L"oplus"),    //!< circled plus = direct sum, U+2295 ISOamsb
    std::make_pair(L'\u2228', L"or"),       //!< logical or = vee, U+2228 ISOtech
    std::make_pair(L'\u00aa', L"ordf"),     //!< feminine ordinal indicator, U+00AA ISOnum
    std::make_pair(L'\u00ba', L"ordm"),     //!< masculine ordinal indicator, U+00BA ISOnum
    std::make_pair(L'\u00f8', L"oslash"),   //!< latin small letter o with stroke, = latin small letter o slash, U+00F8 ISOlat1
    std::make_pair(L'\u00f5', L"otilde"),   //!< latin small letter o with tilde, U+00F5 ISOlat1
    std::make_pair(L'\u2297', L"otimes"),   //!< circled times = vector product, U+2297 ISOamsb
    std::make_pair(L'\u00f6', L"ouml"),     //!< latin small letter o with diaeresis, U+00F6 ISOlat1
    std::make_pair(L'\u00b6', L"para"),     //!< pilcrow sign = paragraph sign, U+00B6 ISOnum
    std::make_pair(L'\u2202', L"part"),     //!< partial differential, U+2202 ISOtech
    std::make_pair(L'\u2030', L"permil"),   //!< per mille sign, U+2030 ISOtech
    std::make_pair(L'\u22a5', L"perp"),     //!< up tack = orthogonal to = perpendicular, U+22A5 ISOtech
    std::make_pair(L'\u03c6', L"phi"),      //!< greek small letter phi, U+03C6 ISOgrk3
    std::make_pair(L'\u03c0', L"pi"),       //!< greek small letter pi, U+03C0 ISOgrk3
    std::make_pair(L'\u03d6', L"piv"),      //!< greek pi symbol, U+03D6 ISOgrk3
    std::make_pair(L'\u00b1', L"plusmn"),   //!< plus-minus sign = plus-or-minus sign, U+00B1 ISOnum
    std::make_pair(L'\u00a3', L"pound"),    //!< pound sign, U+00A3 ISOnum
    std::make_pair(L'\u2032', L"prime"),    //!< prime = minutes = feet, U+2032 ISOtech
    std::make_pair(L'\u220f', L"prod"),     //!< n-ary product = product sign, U+220F ISOamsb
    std::make_pair(L'\u221d', L"prop"),     //!< proportional to, U+221D ISOtech
    std::make_pair(L'\u03c8', L"psi"),      //!< greek small letter psi, U+03C8 ISOgrk3
    std::make_pair(L'\u0022', L"quot"),     //!< quotation mark = APL quote, U+0022 ISOnum
    std::make_pair(L'\u21d2', L"rArr"),     //!< rightwards double arrow, U+21D2 ISOtech
    std::make_pair(L'\u221a', L"radic"),    //!< square root = radical sign, U+221A ISOtech
    std::make_pair(L'\u232a', L"rang"),     //!< right-pointing angle bracket = ket, U+232A ISOtech
    std::make_pair(L'\u00bb', L"raquo"),    //!< right-pointing double angle quotation mark = right pointing guillemet, U+00BB ISOnum
    std::make_pair(L'\u2192', L"rarr"),     //!< rightwards arrow, U+2192 ISOnum
    std::make_pair(L'\u2309', L"rceil"),    //!< right ceiling, U+2309 ISOamsc
    std::make_pair(L'\u201d', L"rdquo"),    //!< right double quotation mark, U+201D ISOnum
    std::make_pair(L'\u211c', L"real"),     //!< blackletter capital R = real part symbol, U+211C ISOamso
    std::make_pair(L'\u00ae', L"reg"),      //!< registered sign = registered trade mark sign, U+00AE ISOnum
    std::make_pair(L'\u230b', L"rfloor"),   //!< right floor, U+230B ISOamsc
    std::make_pair(L'\u03c1', L"rho"),      //!< greek small letter rho, U+03C1 ISOgrk3
    std::make_pair(L'\u200f', L"rlm"),      //!< right-to-left mark, U+200F NEW RFC 2070
    std::make_pair(L'\u203a', L"rsaquo"),   //!< single right-pointing angle quotation mark, U+203A ISO proposed
    std::make_pair(L'\u2019', L"rsquo"),    //!< right single quotation mark, U+2019 ISOnum
    std::make_pair(L'\u201a', L"sbquo"),    //!< single low-9 quotation mark, U+201A NEW
    std::make_pair(L'\u0161', L"scaron"),   //!< latin small letter s with caron, U+0161 ISOlat2
    std::make_pair(L'\u22c5', L"sdot"),     //!< dot operator, U+22C5 ISOamsb
    std::make_pair(L'\u00a7', L"sect"),     //!< section sign, U+00A7 ISOnum
    std::make_pair(L'\u00ad', L"shy"),      //!< soft hyphen = discretionary hyphen, U+00AD ISOnum
    std::make_pair(L'\u03c3', L"sigma"),    //!< greek small letter sigma, U+03C3 ISOgrk3
    std::make_pair(L'\u03c2', L"sigmaf"),   //!< greek small letter final sigma, U+03C2 ISOgrk3
    std::make_pair(L'\u223c', L"sim"),      //!< tilde operator = varies with = similar to, U+223C ISOtech
    std::make_pair(L'\u2660', L"spades"),   //!< black spade suit, U+2660 ISOpub
    std::make_pair(L'\u2282', L"sub"),      //!< subset of, U+2282 ISOtech
    std::make_pair(L'\u2286', L"sube"),     //!< subset of or equal to, U+2286 ISOtech
    std::make_pair(L'\u2211', L"sum"),      //!< n-ary sumation, U+2211 ISOamsb
    std::make_pair(L'\u2283', L"sup"),      //!< superset of, U+2283 ISOtech
    std::make_pair(L'\u00b9', L"sup1"),     //!< superscript one = superscript digit one, U+00B9 ISOnum
    std::make_pair(L'\u00b2', L"sup2"),     //!< superscript two = superscript digit two = squared, U+00B2 ISOnum
    std::make_pair(L'\u00b3', L"sup3"),     //!< superscript three = superscript digit three = cubed, U+00B3 ISOnum
    std::make_pair(L'\u2287', L"supe"),     //!< superset of or equal to, U+2287 ISOtech
    std::make_pair(L'\u00df', L"szlig"),    //!< latin small letter sharp s = ess-zed, U+00DF ISOlat1
    std::make_pair(L'\u03c4', L"tau"),      //!< greek small letter tau, U+03C4 ISOgrk3
    std::make_pair(L'\u2234', L"there4"),   //!< therefore, U+2234 ISOtech
    std::make_pair(L'\u03b8', L"theta"),    //!< greek small letter theta, U+03B8 ISOgrk3
    std::make_pair(L'\u03d1', L"thetasym"), //!< greek small letter theta symbol, U+03D1 NEW
    std::make_pair(L'\u2009', L"thinsp"),   //!< thin space, U+2009 ISOpub
    std::make_pair(L'\u00fe', L"thorn"),    //!< latin small letter thorn with, U+00FE ISOlat1
    std::make_pair(L'\u02dc', L"tilde"),    //!< small tilde, U+02DC ISOdia
    std::make_pair(L'\u00d7', L"times"),    //!< multiplication sign, U+00D7 ISOnum
    std::make_pair(L'\u2122', L"trade"),    //!< trade mark sign, U+2122 ISOnum
    std::make_pair(L'\u21d1', L"uArr"),     //!< upwards double arrow, U+21D1 ISOamsa
    std::make_pair(L'\u00fa', L"uacute"),   //!< latin small letter u with acute, U+00FA ISOlat1
    std::make_pair(L'\u2191', L"uarr"),     //!< upwards arrow, U+2191 ISOnum
    std::make_pair(L'\u00fb', L"ucirc"),    //!< latin small letter u with circumflex, U+00FB ISOlat1
    std::make_pair(L'\u00f9', L"ugrave"),   //!< latin small letter u with grave, U+00F9 ISOlat1
    std::make_pair(L'\u00a8', L"uml"),      //!< diaeresis = spacing diaeresis, U+00A8 ISOdia
    std::make_pair(L'\u03d2', L"upsih"),    //!< greek upsilon with hook symbol, U+03D2 NEW
    std::make_pair(L'\u03c5', L"upsilon"),  //!< greek small letter upsilon, U+03C5 ISOgrk3
    std::make_pair(L'\u00fc', L"uuml"),     //!< latin small letter u with diaeresis, U+00FC ISOlat1
    std::make_pair(L'\u2118', L"weierp"),   //!< script capital P = power set = Weierstrass p, U+2118 ISOamso
    std::make_pair(L'\u03be', L"xi"),       //!< greek small letter xi, U+03BE ISOgrk3
    std::make_pair(L'\u00fd', L"yacute"),   //!< latin small letter y with acute, U+00FD ISOlat1
    std::make_pair(L'\u00a5', L"yen"),      //!< yen sign = yuan sign, U+00A5 ISOnum
    std::make_pair(L'\u00ff', L"yuml"),     //!< latin small letter y with diaeresis, U+00FF ISOlat1
    std::make_pair(L'\u03b6', L"zeta"),     //!< greek small letter zeta, U+03B6 ISOgrk3
    std::make_pair(L'\u200d', L"zwj"),      //!< zero width joiner, U+200D NEW RFC 2070
    std::make_pair(L'\u200c', L"zwnj"),     //!< zero width non-joiner, U+200C NEW RFC 2070
};

} // end of namespace markdown

#endif // HTMLENTITYDEFS_HPP_
