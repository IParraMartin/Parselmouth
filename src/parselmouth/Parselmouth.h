#pragma once
#ifndef INC_PARSELMOUTH_PARSELMOUTH_H
#define INC_PARSELMOUTH_PARSELMOUTH_H

#include "Bindings.h"

#include "dwtools/MFCC.h"
#include "fon/Formant.h"
#include "fon/Harmonicity.h"
#include "fon/Intensity.h"
#include "fon/Pitch.h"
#include "fon/Sound.h"
#include "fon/Spectrogram.h"
#include "fon/Spectrum.h"
#include "sys/Thing.h"
#include "praat/UndefPraatMacros.h"

#include <pybind11/pybind11.h>


PYBIND11_DECLARE_HOLDER_TYPE(T, _Thing_auto<T>);


namespace parselmouth {

// TODO Move to its own header?
template <typename Type>
void make_implicitly_convertible_from_string(pybind11::enum_<Type> &enumType, bool ignoreCase=false)
{
	enumType.def("__init__",
	             [enumType, ignoreCase] (Type &self, const std::string &value)
	             {
		             auto values = enumType.attr("__members__").template cast<pybind11::dict>();

		             auto strValue = pybind11::str(value);
		             if (values.contains(strValue)) {
			             new (&self) Type(values[strValue].template cast<Type>());
			             return;
		             }

		             if (ignoreCase) {
			             auto upperStrValue = strValue.attr("upper")();
			             for (auto &item : values) {
				             if (item.first.attr("upper")().attr("__eq__")(upperStrValue)) {
					             new (&self) Type(item.second.template cast<Type>());
					             return;
				             }
			             }
		             }

		             throw pybind11::value_error("\"" + value + "\" is not a valid value for enum type " + enumType.attr("__name__").template cast<std::string>());
	             });
	pybind11::implicitly_convertible<std::string, Type>();
};


template <typename Class, typename... Extra>
using ClassBinding = pybind11::class_<Class, Extra...>;

template <typename Enum>
using EnumBinding = pybind11::enum_<Enum>;


#define CLASS_BINDING(Type, ...) template<> class Binding<Type> : public ClassBinding<__VA_ARGS__> { using Base = ClassBinding<__VA_ARGS__>; public: Binding(pybind11::handle &scope); void init(); };
#define ENUM_BINDING(Type, ...) template<> class Binding<Type> : public EnumBinding<__VA_ARGS__> { using Base = EnumBinding<__VA_ARGS__>; public: Binding(pybind11::handle &scope); void init(); };
#define BINDING_CONSTRUCTOR(Type, ...) inline Binding<Type>::Binding(pybind11::handle &scope) : Base{scope, __VA_ARGS__} {}

#define PRAAT_CLASS_BINDING(Type, ...) CLASS_BINDING(Type, struct##Type, auto##Type, Type##_Parent) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_CLASS_BINDING_BASE(Type, Base, ...) CLASS_BINDING(Type, struct##Type, auto##Type, struct##Base) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING(Type, ...) ENUM_BINDING(Type, Type) BINDING_CONSTRUCTOR(Type, #Type, __VA_ARGS__)
#define PRAAT_ENUM_BINDING_ALIAS(Alias, Type, ...) using Alias = Type; PRAAT_ENUM_BINDING(Alias, __VA_ARGS__)

#define NO_BINDING_INIT(Type) inline void Binding<Type>::init() {}


enum class Interpolation;

using structData = structDaata;
using Data = Daata;
using autoData = autoDaata;
using Data_Parent = Daata_Parent;


#define PRAAT_CLASSES            \
        Thing,                   \
        Data,                    \
        Vector,                  \
        Sound,                   \
        Spectrum,                \
        Spectrogram,             \
        Pitch,                   \
        Intensity,               \
        Harmonicity,             \
        Formant,                 \
        MFCC

#define PRAAT_ENUMS              \
        Interpolation,           \
        WindowShape,             \
        AmplitudeScaling,        \
        SignalOutsideTimeDomain


CLASS_BINDING(Thing, structThing, autoThing)
BINDING_CONSTRUCTOR(Thing, "Thing")

PRAAT_CLASS_BINDING(Data)
PRAAT_CLASS_BINDING_BASE(Vector, Data, pybind11::buffer_protocol()) // TODO Expose bindings for Matrix
PRAAT_CLASS_BINDING(Sound)
PRAAT_CLASS_BINDING_BASE(Spectrum, Data) // TODO Expose bindings for Matrix
PRAAT_CLASS_BINDING_BASE(Spectrogram, Data) // TODO Expose bindings for Matrix
PRAAT_CLASS_BINDING_BASE(Pitch, Data) // TODO Expose bindings for Sampled
PRAAT_CLASS_BINDING(Intensity)
PRAAT_CLASS_BINDING(Harmonicity)
PRAAT_CLASS_BINDING_BASE(Formant, Data) // TODO Expose bindings for Sampled
PRAAT_CLASS_BINDING_BASE(MFCC, Data) // TODO Expose bindings for CC & Sampled

PRAAT_ENUM_BINDING(Interpolation)
PRAAT_ENUM_BINDING_ALIAS(WindowShape, kSound_windowShape)
PRAAT_ENUM_BINDING_ALIAS(AmplitudeScaling, kSounds_convolve_scaling)
PRAAT_ENUM_BINDING_ALIAS(SignalOutsideTimeDomain, kSounds_convolve_signalOutsideTimeDomain)

NO_BINDING_INIT(Spectrum)
NO_BINDING_INIT(Spectrogram)
NO_BINDING_INIT(Pitch)
NO_BINDING_INIT(Intensity)
NO_BINDING_INIT(Harmonicity)
NO_BINDING_INIT(Formant)
NO_BINDING_INIT(MFCC)


using PraatBindings = Bindings<PRAAT_ENUMS, PRAAT_CLASSES>;

} // parselmouth

#endif // INC_PARSELMOUTH_PARSELMOUTH_H