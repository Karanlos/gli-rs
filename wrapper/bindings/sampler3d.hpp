/// @brief Include to sample 3d textures.
/// @file gli/sampler3d.hpp

#pragma once

#include "sampler.hpp"
#include "texture3d.hpp"
#include "mipmaps_compute.hpp"
#include "convert_func.hpp"

#include "clear.hpp"

namespace gli
{
	/// 3d texture sampler
	/// @tparam T Sampler can fetch, write and interpret any texture format but will expose and process the data through type T conversions.
	/// @tparam P Precision in term of ULPs
	template <typename T, qualifier P = defaultp>
	class sampler3d : public sampler
	{
	private:
		typedef typename detail::interpolate<T>::type interpolate_type;

	public:
		typedef texture3d texture_type;
		typedef typename texture_type::size_type size_type;
		typedef typename texture_type::extent_type extent_type;
		typedef interpolate_type level_type;
		typedef vec<3, interpolate_type, P> normalized_type;
		typedef vec<4, T, P> texel_type;

		sampler3d(texture_type const& Texture, wrap Wrap, filter Mip = FILTER_NEAREST, filter Min = FILTER_NEAREST, texel_type const& BorderColor = texel_type(0, 0, 0, 1));

        void set_border_color(texel_type BorderColor) {
            this->BorderColor = BorderColor;
        }

		/// Access the sampler texture object
		texture_type const& operator()() const;

		/// Fetch a texel from the sampler texture
		texel_type texel_fetch(extent_type const& TexelCoord, size_type const& Level) const;

		/// Write a texel in the sampler texture
		void texel_write(extent_type const& TexelCoord, size_type const & Level, texel_type const& Texel);

		/// Clear the sampler texture with a uniform texel
		void clear(texel_type const& Texel);

		/// Sample the sampler texture at a specific level
		texel_type texture_lod(normalized_type const& SampleCoord, level_type Level) const;

		/// Generate all the mipmaps of the sampler texture from the texture base level
		void generate_mipmaps(filter Minification);

		/// Generate the mipmaps of the sampler texture from the texture base level to the texture max level included
		void generate_mipmaps(size_type BaseLevel, size_type MaxLevel, filter Minification);

	private:
		typedef typename detail::convert<texture_type, T, P>::func convert_type;
		typedef typename detail::convert<texture_type, T, P>::fetchFunc fetch_type;
		typedef typename detail::convert<texture_type, T, P>::writeFunc write_type;
		typedef typename detail::filterBase<detail::DIMENSION_3D, texture_type, interpolate_type, normalized_type, fetch_type, texel_type>::filterFunc filter_type;

		texture_type Texture;
		convert_type Convert;
		texel_type BorderColor;
		filter_type Filter;
	};

	typedef sampler3d<float> fsampler3D;
	typedef sampler3d<double> dsampler3D;
	typedef sampler3d<unsigned int> usampler3D;
	typedef sampler3d<int> isampler3D;
}//namespace gli


extern "C" {

    namespace bindings {

        namespace FSampler3D {

#ifndef _WIN32
            gli::fsampler3D fsampler3d_new(const gli::texture3d & Texture, gli::wrap Wrap, gli::filter Mip, gli::filter Min) {
                return gli::fsampler3D(Texture, Wrap, Mip, Min);
            }
#endif

            void fsampler3d_set_border_color(gli::fsampler3D & Sampler, TexelType4F BorderColor) {
                Sampler.set_border_color(gli::tex4FToVec4(BorderColor));
            }

            void fsampler3d_clear(gli::fsampler3D & Sampler, TexelType4F Texel) {
                Sampler.clear(gli::tex4FToVec4(Texel));
            }

            TexelType4F fsampler3d_texel_fetch(const gli::fsampler3D & Sampler, gli::fsampler3D::extent_type TexelCoord, gli::texture::size_type Level) {
                gli::vec4 raw = Sampler.texel_fetch(TexelCoord, Level);
                return vec4ToTex4F(raw);
            }

            void fsampler3d_texel_write(gli::fsampler3D & Sampler, gli::fsampler3D::extent_type TexelCoord, gli::texture::size_type Level, TexelType4F Texel) {
                return Sampler.texel_write(TexelCoord, Level, gli::tex4FToVec4(Texel));
            }

            TexelType4F fsampler3d_texel_lod(const gli::fsampler3D & Sampler, const float SampleCoord[3], float Level) {
                gli::vec4 raw = Sampler.texture_lod(gli::fsampler3D::normalized_type(SampleCoord[0], SampleCoord[1], SampleCoord[2]), Level);
                return vec4ToTex4F(raw);
            }

            const gli::texture3d & fsampler3d_target_texture(const gli::fsampler3D & Sampler) {
                return Sampler.operator()();
            }

            void fsampler3d_generate_mipmaps1(gli::fsampler3D & Sampler, gli::filter Minification) {
                Sampler.generate_mipmaps(Minification);
            }

            void fsampler3d_generate_mipmaps3(gli::fsampler3D & Sampler, gli::texture::size_type BaseLevel, gli::texture::size_type  MaxLevel, gli::filter Minification) {
                Sampler.generate_mipmaps(BaseLevel, MaxLevel, Minification);
            }
        }
    }
}

#ifdef GLI_IMPLEMENTATION
#include "sampler3d.inl"
#endif
