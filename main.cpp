#include <iomanip>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "config.h"

const std::string defaultThumbnailFilename = THUMBNAIL_FILENAME;
const float ratioPptPerCm = 360000.f;
const cv::Size thumbnailSize = cv::Size(256, 184);

class mark
{
public:
    mark(enum markerType t, float gap, float size)
        : mType(t)
        , mGap(gap)
        , mSize(size) {};
    std::string locateMark(int x, int y) const;
protected:
    enum markerType mType;
    float mGap;
    float mSize;
};

class calibrationPattern: public mark
{
public:
    calibrationPattern(enum markerType t, float gap, float size, int columns, int rows)
        : mark(t, gap, size)
    {
        patternSize.width = columns;
        patternSize.height = rows;
    };
    void generateThumbnail(const std::string& filename);
    friend std::ostream& operator << (std::ostream& o, const calibrationPattern& a);
protected:
    cv::Size patternSize;
};

void calibrationPattern::generateThumbnail(const std::string& filename)
{
    const float thumbnailScale = 40.f;
    const cv::Size thumbnailSrcSize = cv::Size(25, 18) * (int)thumbnailScale;
    const cv::Scalar color = cv::Scalar(0);
    const int lineWidth = -1; // fill the shapes
    const int cBoxLength = (int)(cMarkerGap * thumbnailScale);
    const int cCircleRadius = (int)(mSize * thumbnailScale);
    cv::Mat thumbnail = ~cv::Mat::zeros(thumbnailSrcSize, CV_8UC3);
    switch (cMarkerType)
    {
    case marker_Chessboard:
        for (int y = 0; y < patternSize.height + 1; y++)
        {
            for (int x = 0; x < patternSize.width + 1; x++)
            {
                if (((x ^ y) & 1) == 1)
                {
                    cv::rectangle(thumbnail, cv::Rect((int)((x + cOffset) * thumbnailScale), (int)((y + cOffset) * thumbnailScale), cBoxLength, cBoxLength), color, lineWidth);
                }
            }
        }
        break;
    case marker_CircleSymmetrical:
        for (int y = 0; y < patternSize.height; y++)
        {
            for (int x = 0; x < patternSize.width; x++)
            {
                cv::circle(thumbnail, cv::Point((int)((x + cOffset) * thumbnailScale), (int)((y + cOffset) * thumbnailScale)), cCircleRadius, color, lineWidth);
            }
        }
        break;
    case marker_CircleAsymmetrical:
    default:
        float asymmetricScale = thumbnailScale * 2.f;
        for (int y = 0; y < patternSize.height; y++)
        {
            for (int x = 0; x < patternSize.width; x++)
            {
                float xOffset = (y & 1) ? 0.5f : 0.f;
                cv::circle(thumbnail, cv::Point((int)((x + xOffset + cOffset) * asymmetricScale), (int)((y + cOffset) * asymmetricScale / 2)), cCircleRadius, color, lineWidth);
            }
        }
        break;
    }
    cv::resize(thumbnail.clone(), thumbnail, thumbnailSize);
    cv::imwrite(filename, thumbnail);
}

std::string mark::locateMark(int x, int y) const
{
    static int objectCounter = 3;
    std::stringstream ss;
    std::string shapeType = "ellipse";
    int outx = (int)((x * mGap + cOffset) * ratioPptPerCm);
    int outy = (int)((y * mGap + cOffset) * ratioPptPerCm);
    int outcx = (int)(mSize * ratioPptPerCm);
    int outcy = outcx;
    float asymmetricalOffset = 0.f;

    switch (mType)
    {
    default:
    case marker_Chessboard:
        if (((x ^ y) & 1) == 0)
        {
            return ss.str();
        }
        outcx = outcy = (int)(mGap * ratioPptPerCm);
        shapeType = "rect";
        break;
    case marker_CircleSymmetrical:
        break;
    case marker_CircleAsymmetrical:
        if ((y & 1) == 1)
        {
            asymmetricalOffset = mGap / 2;
        }
        outx = (int)((x * mGap + cOffset + asymmetricalOffset) * ratioPptPerCm);
        outy = (int)((y * mGap / 2 + cOffset) * ratioPptPerCm);
        break;
    }

    ss << "<p:sp><p:nvSpPr>";
    ss << "<p:cNvPr id=\"" << objectCounter + 1 << "\" name=\"" << shapeType << " " << objectCounter << "\"><a:extLst><a:ext uri = \"{FF2B5EF4-FFF2-40B4-BE49-F238E27FC236}\">";
    ss << "<a16:creationId xmlns:a16=\"http://schemas.microsoft.com/office/drawing/2014/main\" id=\"{0B28A292-7EE8-4F92-8CEE-EB75EAFE" << std::hex << std::setw(2) << std::setfill('0') << x << std::setw(2) << std::setfill('0') << y << std::dec << "}\"/></a:ext></a:extLst></p:cNvPr><p:cNvSpPr/><p:nvPr/></p:nvSpPr><p:spPr><a:xfrm>";
    ss << "<a:off x=\"" << outx << "\" y=\"" << outy << "\"/><a:ext cx=\"" << outcx << "\" cy=\"" << outcy << "\"/></a:xfrm>";
    ss << "<a:prstGeom prst=\"" << shapeType << "\">";
    ss << "<a:avLst/></a:prstGeom><a:solidFill><a:schemeClr val=\"tx1\"/></a:solidFill><a:ln><a:noFill/></a:ln></p:spPr><p:style><a:lnRef idx=\"2\"><a:schemeClr val=\"accent1\"><a:shade val=\"50000\"/></a:schemeClr></a:lnRef><a:fillRef idx=\"1\"><a:schemeClr val=\"accent1\"/></a:fillRef><a:effectRef idx=\"0\"><a:schemeClr val=\"accent1\"/></a:effectRef><a:fontRef idx=\"minor\"><a:schemeClr val=\"lt1\"/></a:fontRef></p:style><p:txBody><a:bodyPr rtlCol=\"0\" anchor=\"ctr\"/><a:lstStyle/><a:p><a:pPr algn=\"ctr\"/><a:endParaRPr kumimoji=\"1\" lang=\"ja-JP\" altLang=\"en-US\"/></a:p></p:txBody></p:sp>";
    objectCounter++;
    return ss.str();
}

std::ostream& operator << (std::ostream& o, const calibrationPattern& a)
{
    o << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
    o << "<p:sld xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\">";
    o << "<p:cSld><p:spTree><p:nvGrpSpPr><p:cNvPr id=\"1\" name=\"\"/><p:cNvGrpSpPr/><p:nvPr/></p:nvGrpSpPr><p:grpSpPr><a:xfrm><a:off x=\"0\" y=\"0\"/><a:ext cx=\"0\" cy=\"0\"/><a:chOff x=\"0\" y=\"0\"/><a:chExt cx=\"0\" cy=\"0\"/></a:xfrm></p:grpSpPr>";

    int cColumn = a.patternSize.width;
    int cRow = a.patternSize.height;
    if (a.mType == marker_Chessboard)
    {
        cColumn++;
        cRow++;
    }

    for (int y = 0; y < cRow; y++)
    {
        for (int x = 0; x < cColumn; x++)
        {
            o << a.locateMark(x, y);
        }
    }

    o << "</p:spTree><p:extLst><p:ext uri=\"{BB962C8B-B14F-4D97-AF65-F5344CB8AC3E}\"><p14:creationId xmlns:p14=\"http://schemas.microsoft.com/office/powerpoint/2010/main\" val=\"269148240\"/></p:ext></p:extLst></p:cSld><p:clrMapOvr><a:masterClrMapping/></p:clrMapOvr>";
    o << "</p:sld>" << std::endl;

    return o;
}

int main()
{
    std::cerr << "Opening " << DEFAULT_XML_FILENAME << "..." << std::endl;

    calibrationPattern pattern(cMarkerType, cMarkerGap, cSizeCircle, cNumMarkerCols, cNumMarkerRows);
    std::ofstream ofs(DEFAULT_XML_FILENAME);
    ofs << pattern;
    pattern.generateThumbnail(defaultThumbnailFilename);

    return 0;
}
