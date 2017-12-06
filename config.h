#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED

/* number of markers in column */
const int cNumMarkerCols = 4;

/* number of markers in row */
const int cNumMarkerRows = 9;

/* gap between markers in cm (= size of each box of chessboard) */
const float cMarkerGap = 1.0f; // in cm

/* diameter of the circle in cm (not used for chessboard) */
const float cSizeCircle = 0.5f; // in cm

/* offset from the edge in cm */
const float cOffset = 1.0f; // in cm

/* name of powerpoint slide file name */
#define DEFAULT_XML_FILENAME "slide1.xml"

/* name of thumbnail file */
#define THUMBNAIL_FILENAME "thumbnail.jpeg"

enum markerType
{
    marker_Chessboard,
    marker_CircleSymmetrical,
    marker_CircleAsymmetrical,
};

const enum markerType cMarkerType = marker_Chessboard;

#endif // CONFIGURE_H_INCLUDED
