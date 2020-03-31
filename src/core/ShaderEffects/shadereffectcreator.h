// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SHADEREFFECTCREATOR_H
#define SHADEREFFECTCREATOR_H
#include "shadereffectprogram.h"

class ShaderEffect;

struct CORE_EXPORT ShaderEffectCreator : public StdSelfRef {
    e_OBJECT
public:
    ShaderEffectCreator(const QString& grePath, const QString& name,
                        const QList<stdsptr<ShaderPropertyCreator>>& propCs,
                        std::unique_ptr<ShaderEffectProgram>&& program) :
        fName(name), fGrePath(grePath),
        fProperties(propCs), fProgram(std::move(program)) {}

    struct Identifier {
        Identifier(const QString& grePath, const QString& name,
                   const QList<ShaderPropertyType>& types) :
        fGrePath(grePath), fName(name), fTypes(types) {}

        const QString fGrePath;
        const QString fName;
        const QList<ShaderPropertyType> fTypes;
    };

    const QString fName;
    const QString fGrePath;
    const QList<stdsptr<ShaderPropertyCreator>> fProperties;
    std::unique_ptr<ShaderEffectProgram> fProgram;

    bool compatible(const QList<ShaderPropertyType> &props) const;

    void reloadProgram(QGL33 * const gl, const QString& fragPath);

    qsptr<ShaderEffect> create() const;

    void writeIdentifier(eWriteStream& dst) const;

    static Identifier sReadIdentifier(eReadStream& src);

    static stdsptr<ShaderEffectCreator> sLoadFromFile(
            QGL33 * const gl, const QString& grePath);

    static stdsptr<ShaderEffectCreator> sWithGrePath(
            const QString& grePath);

    static stdsptr<ShaderEffectCreator> sWithGrePathAndCompatible(
            const QString& grePath,
            const QList<ShaderPropertyType>& props);

    static QList<stdsptr<ShaderEffectCreator>> sWithName(
            const QString &name);

    static QList<stdsptr<ShaderEffectCreator>> sWithNameAndCompatible(
            const QString &name,
            const QList<ShaderPropertyType>& props);

    static QList<stdsptr<ShaderEffectCreator>> sWithCompatibleProps(
            const QList<ShaderPropertyType>& props);

    static QList<stdsptr<ShaderEffectCreator>> sGetBestCompatibleEffects(
            const Identifier& id);

    template <typename T> using Func = std::function<T>;
    template <typename T> using Creator = Func<qsptr<T>()>;
    using CCreator = Creator<ShaderEffect>;
    using CAdder = Func<void(const QString&, const CCreator&)>;
    static void sForEveryEffect(const CAdder& add) {
        for(const auto& creator : sEffectCreators) {
            const auto cCreator = [creator]() { return creator->create(); };
            add(creator->fName, cCreator);
        }
    }

    static QList<stdsptr<ShaderEffectCreator>> sEffectCreators;
};

#endif // SHADEREFFECTCREATOR_H
