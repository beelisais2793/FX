/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== Speaker_Icon.svg ==================
static const unsigned char temp_binary_data_0[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<svg version=\"1.0\" width=\"500\" height=\"500\" viewBox=\"0 0 75 75\" xmlns=\"http://www.w3.org/2000/svg\">\n"
"  <path d=\"M39.389,13.769 L22.235,28.606 L6,28.606 L6,47.699 L21.989,47.699 L39.389,62.75 L39.389,13.769z\" style=\"stroke-width: 5; stroke-linejoin: round; stroke: rgb(0, 0, 0);\"/>\n"
"  <path d=\"M48,27.6a19.5,19.5 0 0 1 0,21.4M55.1,20.5a30,30 0 0 1 0,35.6M61.6,14a38.8,38.8 0 0 1 0,48.6\" style=\"fill: none; stroke-width: 5; stroke-linecap: round; stroke: rgb(0, 0, 0);\"/>\n"
"</svg>";

const char* Speaker_Icon_svg = (const char*) temp_binary_data_0;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x89e74e1e:  numBytes = 513; return Speaker_Icon_svg;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "Speaker_Icon_svg"
};

const char* originalFilenames[] =
{
    "Speaker_Icon.svg"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
    {
        if (namedResourceList[i] == resourceNameUTF8)
            return originalFilenames[i];
    }

    return nullptr;
}

}
