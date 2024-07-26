package org.jetbrains.skia

import org.jetbrains.skia.impl.use
import org.jetbrains.skia.tests.assertCloseEnough
import org.jetbrains.skia.tests.assertContentCloseEnough
import org.jetbrains.skia.tests.makeFromResource
import org.jetbrains.skiko.KotlinBackend
import org.jetbrains.skiko.OS
import org.jetbrains.skiko.hostOs
import org.jetbrains.skiko.kotlinBackend
import org.jetbrains.skiko.tests.runTest
import kotlin.test.*

private fun isMac() = (hostOs == OS.MacOS)
private fun isIos() = (hostOs == OS.Ios)
private fun isLinux() = (hostOs == OS.Linux)
private fun isWindows() = (hostOs == OS.Windows)
private fun isTvos() = (hostOs == OS.Tvos)
private fun isJs() = (kotlinBackend == KotlinBackend.JS)
private val COARSE_EPSILON = 2.4f
private const val jbMonoPath = "./fonts/JetBrainsMono-Regular.ttf"

class FontTests {
    @Test
    fun fontTest() = runTest {
        val jbMono = Typeface.makeFromResource(jbMonoPath)
        Font(jbMono).use { font ->
            assertEquals(12f, font.size)
            // TODO: we have to use bigger epsilon because of MacOS and definitely need to investigate what would be a better solution
            assertCloseEnough(14.880001f, font.spacing, 10e-5f)

            val glyphs = font.getStringGlyphs("ABCDE")
            assertContentEquals(shortArrayOf(17, 18, 19, 20, 21), glyphs)

            assertEquals(6, font.getStringGlyphsCount("EЙ를üẞ無"))
            assertCloseEnough(50.4f, font.measureTextWidth("EЙ를üẞՇ無"), COARSE_EPSILON)

            assertContentCloseEnough(floatArrayOf(7.2f, 7.2f, 7.2f, 7.2f, 7.2f), font.getWidths(glyphs), COARSE_EPSILON)

            assertContentCloseEnough(floatArrayOf(0f, 7.2f, 14.4f, 21.6f, 28.8f), font.getXPositions(glyphs), COARSE_EPSILON)
            assertContentCloseEnough(floatArrayOf(3f, 10.2f, 17.4f, 24.6f, 31.8f), font.getXPositions(glyphs, 3f), COARSE_EPSILON)

            val firstGlyphPath = font.getPath(glyphs[0])!!
            assertContentCloseEnough(
                listOf(
                    Point(2.8798828f, -8.639648f),
                    Point(4.3916016f, -8.639648f),
                    Point(6.6708984f, 0.0f),
                    Point(5.5195312f, 0.0f),
                    Point(4.9189453f, -2.4345703f)
                ),
                // TODO: investigate why we have nullable points at all
                (firstGlyphPath.points.toList() as List<Point>).subList(0, 5),
                10e-3f
            )

            assertContentEquals(font.getPath(glyphs[0])!!.points,  font.getPaths(glyphs)[0].points)

            assertContentCloseEnough(
                arrayOf(
                    Point(0f, 0f),
                    Point(7.2f, 0f),
                    Point(14.4f, 0f),
                    Point(21.6f, 0f),
                    Point(28.8f, 0f),
                ), font.getPositions(glyphs),
                COARSE_EPSILON
            )

            assertContentCloseEnough(
                arrayOf(
                    Point(3f, 2f),
                    Point(10.2f, 2f),
                    Point(17.4f, 2f),
                    Point(24.6f, 2f),
                    Point(31.8f, 2f),
                ), font.getPositions(glyphs, Point(3f, 2f)),
                COARSE_EPSILON
            )

            val expectedGlyphBounds = arrayOf(
                Rect(-1f, -10.0f, 8.0f, 1.0f),
                Rect(0f, -10.0f, 8.0f, 1.0f),
                Rect(0f, -10.0f, 8.0f, 2.0f),
                Rect(0f, -10.0f, 8.0f, 1.0f),
                Rect(0f, -10.0f, 8.0f, 1.0f),
            )

            expectedGlyphBounds.zip(font.getBounds(glyphs)).forEach { (expected, actual) ->
                assertCloseEnough(expected, actual, COARSE_EPSILON)
            }

            assertEquals(if (isLinux() || isJs()) 26 else 24, firstGlyphPath.pointsCount)

            //FontMetrics(_top=-11.64, _ascent=-11.64, _descent=3.24, _bottom=3.24, _leading=0.0, _avgCharWidth=29.46, _maxCharWidth=29.46, _xMin=-20.88, _xMax=8.58, _xHeight=6.6, _capHeight=8.64, _underlineThickness=0.54, _underlinePosition=1.44, _strikeoutThickness=0.54, _strikeoutPosition=-3.9), eps=0.01
            //FontMetrics(_top=-11.64, _ascent=-11.64, _descent=3.2400002, _bottom=3.2400002, _leading=0.0, _avgCharWidth=7.2, _maxCharWidth=29.460001, _xMin=-20.880001, _xMax=8.58, _xHeight=6.6000004, _capHeight=8.64, _underlineThickness=0.54, _underlinePosition=1.4399999, _strikeoutThickness=0.54, _strikeoutPosition=-3.8999999)
            assertCloseEnough(FontMetrics(
                top = -11.64f,
                ascent = -11.64f,
                descent = 3.2400002f,
                bottom = 3.2400002f,
                leading = 0f,
                avgCharWidth = when {
                    isJs() -> 7.2f
                    isIos() || isMac() || isTvos() -> 29.460001f
                    isWindows() -> 0f
                    else -> 7.2f
                },
                maxCharWidth = 29.460001f,
                xMin = -20.880001f,
                xMax = 8.58f,
                xHeight = 6.6000004f,
                capHeight = 8.64f,
                underlineThickness = 0.54f,
                underlinePosition = 1.4399999f,
                strikeoutThickness = 0.54f,
                strikeoutPosition = -3.8999999f
            ), font.metrics, 10e-3f)

//            assertEquals(Rect(1f, -12f, 21f, 0f), font.measureText("ЕЁЫ"))

        }
    }

    @Test
    fun fontLinearMetricsTest() = runTest {
        val jbMono = Typeface.makeFromResource(jbMonoPath)
        Font(jbMono).use { font ->
            assertFalse(font.isLinearMetrics)
            font.isLinearMetrics = true
            assertTrue(font.isLinearMetrics)
        }
    }

    @Test
    // Maybe it's a reason why this test fails with m122:
    // https://github.com/google/skia/blob/main/RELEASE_NOTES.md#milestone-122
    // SkFont::getTypeface() will no longer return a nullptr to indicate "the default typeface".
    // If left unspecified, SkFonts will use an empty typeface (e.g. no glyphs).
    fun emptyFontMetrics() {
        Font(null).use { font ->
            val metrics = font.metrics
            assertFalse(
                metrics.top == 0f &&
                        metrics.bottom == 0f &&
                        metrics.ascent == 0f &&
                        metrics.descent == 0f
            )
        }
    }
}
