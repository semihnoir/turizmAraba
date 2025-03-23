"use client";

import { useState, useEffect } from "react";
import Image from "next/image";

export default function Home() {
  const [bolgeAdi, setBolgeAdi] = useState<string | null>(null);
  const [sesNumarasi, setSesNumarasi] = useState<number | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [ipAdresi, setIpAdresi] = useState<string>(""); 
  const [audioElement, setAudioElement] = useState<HTMLAudioElement | null>(null);

  const mp3Url = sesNumarasi !== null 
    ? `/mp3/${sesNumarasi < 10 
        ? `000${sesNumarasi}` 
        : sesNumarasi < 100 
          ? `00${sesNumarasi}` 
          : `0${sesNumarasi}`}.mp3` 
    : null;

  const fetchData = async () => {
    if (!ipAdresi) return;
    try {
      const response = await fetch(`/api/data?ip=${ipAdresi}`);
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      setBolgeAdi(data.bolgeAdi);
      setSesNumarasi(data.sesNumarasi);
      setError(null);
    } catch (e: any) {
      setError(`Veri çekme hatası: ${e.message}`);
      console.error("Veri çekme hatası:", e);
    }
  };

  useEffect(() => {
    if (ipAdresi) {
      fetchData();
      const intervalId = setInterval(fetchData, 5000);
      return () => clearInterval(intervalId);
    }
  }, [ipAdresi]);

  useEffect(() => {
    if (mp3Url && audioElement) {
      audioElement.load();
      audioElement.play().catch(error => {
        console.error("Ses oynatma hatası:", error);
      });
    }
  }, [mp3Url, audioElement]);

  const handleIpAdresiChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setIpAdresi(event.target.value);
  };

  const getBolgeRengi = (bolge: string) => {
    return bolge === bolgeAdi ? 'bg-blue-500' : 'bg-gray-200';
  };

  return (
    <div className="min-h-screen bg-gradient-to-b from-blue-50 to-white">
      <div className="container mx-auto px-4 py-8">
        <div className="max-w-4xl mx-auto">
          {/* Başlık */}
          <h1 className="text-3xl font-bold text-center mb-8 text-blue-800">
            Türkiye Turizm Rehberi
          </h1>

          {/* IP Giriş Alanı */}
          <div className="bg-white p-6 rounded-lg shadow-md mb-8">
            <div className="flex gap-4 items-center">
              <label htmlFor="ipAdresi" className="font-medium text-gray-700">
                Arduino IP Adresi:
              </label>
              <input
                type="text"
                id="ipAdresi"
                placeholder="Örn: 192.168.1.100"
                value={ipAdresi}
                onChange={handleIpAdresiChange}
                className="flex-1 border border-gray-300 rounded-md px-4 py-2 focus:outline-none focus:ring-2 focus:ring-blue-500 text-black"
              />
            </div>
          </div>

          {/* Hata Mesajı */}
          {error && (
            <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-8">
              {error}
            </div>
          )}

          {/* Ana İçerik */}
          <div className="grid md:grid-cols-2 gap-8">
            {/* Sol Taraf - Bölge Bilgisi ve Ses */}
            <div className="bg-white p-6 rounded-lg shadow-md">
              <h2 className="text-xl font-semibold mb-4 text-gray-800">
                Mevcut Konum Bilgisi
              </h2>
              <div className="mb-6">
                <p className="text-lg font-medium text-blue-600">
                  Bulunduğunuz Bölge: {bolgeAdi || "Bilinmiyor"}
                </p>
              </div>
              {mp3Url && (
                <div className="mt-4">
                  <h3 className="text-lg font-medium mb-2 text-gray-700">
                    Bölge Tanıtım Sesi
                  </h3>
                  <audio
                    controls
                    className="w-full"
                    ref={setAudioElement}
                    autoPlay
                  >
                    <source src={mp3Url} type="audio/mpeg" />
                    Tarayıcınız ses etiketini desteklemiyor.
                  </audio>
                </div>
              )}
            </div>

            {/* Sağ Taraf - Türkiye Bölgeleri */}
            <div className="bg-white p-6 rounded-lg shadow-md">
              <h2 className="text-xl font-semibold mb-4 text-gray-800">
                Türkiye Bölgeleri
              </h2>
              <div className="space-y-2">
                {[
                  "Marmara",
                  "Ege",
                  "Akdeniz",
                  "İç Anadolu",
                  "Karadeniz",
                  "Doğu Anadolu",
                  "Güneydoğu Anadolu"
                ].map((bolge) => (
                  <div
                    key={bolge}
                    className={`p-3 rounded-md transition-colors duration-300 ${getBolgeRengi(bolge)} ${
                      bolge === bolgeAdi ? 'text-white' : 'text-gray-700'
                    }`}
                  >
                    {bolge}
                  </div>
                ))}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
