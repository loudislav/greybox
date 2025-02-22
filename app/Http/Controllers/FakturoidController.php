<?php

namespace App\Http\Controllers;
use App\Services\FakturoidClientService;

abstract class FakturoidController extends Controller
{
    /*
     * @return Fakturoid\Client
     * TODO: To be deleted
     */
    public function getFakturoidClient()
    {
        return new FakturoidClientService();
    }

    public function fillInvoiceData(array $data, $fakturoidInvoice)
    {
        $data['fakturoid_id'] = $fakturoidInvoice->id;
        $data['number'] = $fakturoidInvoice->number;
        $data['status'] = $fakturoidInvoice->status;
        $data['issued_on'] = $fakturoidInvoice->issued_on;
        $data['due_on'] = $fakturoidInvoice->due_on;
        $data['currency'] = $fakturoidInvoice->currency;
        $data['language'] = $fakturoidInvoice->language;
        $data['total'] = $fakturoidInvoice->total;

        return $data;
    }
}
