#ifndef BOUNDINGBOXRENDERDATA_H
#define BOUNDINGBOXRENDERDATA_H
#include "skia/skiaincludes.h"

#include <QWeakPointer>
#include "updatable.h"
#include "Animators/animator.h"
#include <QMatrix>
struct PixmapEffectRenderData;
class BoundingBox;
class ShaderProgramCallerBase;
class GPURasterEffectCaller;
#include "smartPointers/sharedpointerdefs.h"
#include "glhelpers.h"

class RenderDataCustomizerFunctor;
struct BoundingBoxRenderData : public Task {
    friend class StdSelfRef;
protected:
    BoundingBoxRenderData(BoundingBox *parentBoxT);

    virtual void drawSk(SkCanvas * const canvas) = 0;
    virtual void setupRenderData() {}
    virtual void transformRenderCanvas(SkCanvas& canvas) const;
    virtual void copyFrom(BoundingBoxRenderData *src);
    virtual void updateRelBoundingRect();

    void scheduleTaskNow() final;
    void afterCanceled();

    virtual SkColor eraseColor() const { return SK_ColorTRANSPARENT; }
public:
    virtual QPointF getCenterPosition() {
        return fRelBoundingRect.center();
    }

    bool gpuProcessingNeeded() const { return !fGPUEffects.isEmpty(); }

    virtual void processTaskWithGPU(QGL33 * const gl,
                                    GrContext * const grContext);
    void processTask();
    void beforeProcessing() final;
    void afterProcessing() final;
    void taskQued() final;

    // gpu
    GpuSupport gpuSupport() const;

    QList<stdsptr<GPURasterEffectCaller>> fGPUEffects;
    // gpu

    stdsptr<BoundingBoxRenderData> makeCopy();
    bool fCopied = false;
    bool fRelBoundingRectSet = false;

    Animator::UpdateReason fReason;

    uint fBoxStateId = 0;

    QMatrix fResolutionScale;
    QMatrix fScaledTransform;
    QMatrix fTransform;
    QMatrix fRenderTransform;

    QRectF fRelBoundingRect;
    QRect fGlobalRect;
    QRect fMaxBoundsRect;

    QMargins fBaseMargin;

    qreal fOpacity = 1;
    qreal fResolution;
    int fRelFrame;

    // for motion blur
    bool fUseCustomRelFrame = false;
    qreal fCustomRelFrame;
    QList<QRectF> fOtherGlobalRects;
    stdptr<BoundingBoxRenderData> fMotionBlurTarget;
    // for motion blur

    QList<stdsptr<PixmapEffectRenderData>> fRasterEffects;
    SkBlendMode fBlendMode = SkBlendMode::kSrcOver;

    bool fParentIsTarget = true;
    bool fRefInParent = false;
    qptr<BoundingBox> fParentBox;
    sk_sp<SkImage> fRenderedImage;

    void drawRenderedImageForParent(SkCanvas * const canvas);

    void dataSet();

    void clearPixmapEffects() {
        fRasterEffects.clear();
    }

    void appendRenderCustomizerFunctor(
            const stdsptr<RenderDataCustomizerFunctor>& customizer) {
        mRenderDataCustomizerFunctors.append(customizer);
    }

    void prependRenderCustomizerFunctor(
            const stdsptr<RenderDataCustomizerFunctor>& customizer) {
        mRenderDataCustomizerFunctors.prepend(customizer);
    }
    bool nullifyBeforeProcessing();
protected:
    virtual void updateGlobalRect();

    void setBaseGlobalRect(const QRectF &baseRectF);

    QList<stdsptr<RenderDataCustomizerFunctor>> mRenderDataCustomizerFunctors;
    bool mDelayDataSet = false;
    bool mDataSet = false;
};

class RenderDataCustomizerFunctor : public StdSelfRef {
public:
    RenderDataCustomizerFunctor();
    virtual void customize(BoundingBoxRenderData * const data) = 0;
    void operator()(BoundingBoxRenderData * const data);
};

class ReplaceTransformDisplacementCustomizer : public RenderDataCustomizerFunctor {
public:
    ReplaceTransformDisplacementCustomizer(const qreal dx,
                                           const qreal dy);

    void customize(BoundingBoxRenderData * const data);
protected:
    qreal mDx, mDy;
};

class MultiplyTransformCustomizer : public RenderDataCustomizerFunctor {
public:
    MultiplyTransformCustomizer(const QMatrix &transform,
                                const qreal opacity = 1);

    void customize(BoundingBoxRenderData * const data);
protected:
    QMatrix mTransform;
    qreal mOpacity = 1;
};

class MultiplyOpacityCustomizer : public RenderDataCustomizerFunctor {
public:
    MultiplyOpacityCustomizer(const qreal opacity);

    void customize(BoundingBoxRenderData * const data);
protected:
    qreal mOpacity;
};

#endif // BOUNDINGBOXRENDERDATA_H